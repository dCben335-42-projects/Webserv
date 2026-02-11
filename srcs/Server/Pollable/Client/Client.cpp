#include "Client.hpp"
#include "httpExceptions.hpp"
#include "CgiSpawner.hpp"
#include "httpMimes.hpp"
#include "FileHandler.hpp"
#include "FileUpload.hpp"
#include "Date.hpp"
#include "IndexOf.hpp"
#include "Logger.hpp"
#include "EventLoop.hpp"
#include "webserv.hpp"

Client::Client(int fd, const Config &config) :
	Pollable(fd),
	m_serverConfig(config)
{
	m_count = 0;
	reset();
	EventLoop::addHandler(this);
}

Client::~Client()
{
	if (m_fd != -1)
		close(m_fd);
}

/**
 *
 * @brief Resets the client's state.
 *
 * This function clears the HTTP request and response objects,
 * resets the timeout state, and prepares the client for a new request.
 *
 * It is typically called after a response has been fully sent
 * and the client is ready to handle a new request.
 *
 */
void Client::reset()
{
	m_req.reset();
	m_res.reset();
	m_timeoutState = KEEP_ALIVE_TIMEOUT;
	m_shouldClose  = false;
	m_events	   = POLLIN;
	m_time		   = std::time(0);
}

/**
 *
 * @brief Gets the HTTP request associated with the client.
 *
 * @note It is used to access the request details and headers.
 *
 * @return A constant reference to the HttpRequest object.
 *
 */
const HttpRequest &Client::getRequest() const
{
	return m_req;
}

/**
 *
 * @brief Gets the HTTP response associated with the client.
 *
 * @note You can modify the response using the returned reference.
 * It is useful for preparing and sending responses to the client
 * from the CgiPipe or other components.
 *
 * @return A non-constant reference to the HttpResponse object.
 *
 */
HttpResponse &Client::getResponse()
{
	return m_res;
}

/**
 *
 * @brief Sets the events for the client.
 *
 * @note This method updates the event flags
 * used for polling the client socket.
 *
 * @see Pollable::setEvents(short events)
 *
 * @param events The events to be set.
 * (e.g., POLLIN, POLLOUT)
 *
 */
void Client::setEvents(const short events)
{
	m_events = events;
}

/**
 *
 * @brief Handles HTTP POST requests.
 *
 * This function processes the POST request,
 * retrieves the submitted data,
 * and prepares the HTTP response with the appropriate headers and body.
 *
 * @throws AHttpException if there are issues with processing the POST data.
 *
 */
void Client::handlePost()
{
	FileUpload file(m_req);

	if (!m_req.hasUpload())
		throw Http403Exception();
	if (!file.isUploadValid())
		throw Http400Exception();
	m_req.setBody("File uploaded successfully.");
	m_res.setHeader(HEADER_CONTENT_TYPE, MIME_PLAIN);
	m_res.setStatusCode(201);
}


/**
 *
 * @brief Handles HTTP PUT and PATCH requests.
 *
 * This function processes the PUT or PATCH request,
 * checks for the existence and permissions of the target resource,
 * and updates the file with the provided data.
 *
 * @param ressourcePath The path to the resource being updated.
 *
 * @throws AHttpException if there are issues with accessing or modifying the resource.
 *
 */
void Client::handlePutPatch(const std::string &ressourcePath)
{
	const char *fullPath = ressourcePath.c_str();
	if (access(fullPath, F_OK) == -1)
		throw Http404Exception();
	if (access(fullPath, R_OK | W_OK) == -1)
		throw Http403Exception();
	FileUpload file(m_req);
	file.createDiskFile(ressourcePath, m_req.getBody());
	m_res.setStatusCode(204);
	m_res.setBody("File edited with a lot of success");
	m_res.setHeader(HEADER_CONTENT_TYPE, MIME_PLAIN);
}



/**
 *
 * @brief Handles HTTP DELETE requests.
 *
 * This function processes the DELETE request,
 * checks for the existence and permissions of the target resource,
 * and deletes the file if allowed.
 *
 * @param ressourcePath The path to the resource being deleted.
 *
 * @throws AHttpException if there are issues with accessing or deleting the resource.
 *
 */
void Client::handleDelete(const std::string &ressourcePath)
{
	const char *fullPath = ressourcePath.c_str();
	if (access(fullPath, F_OK) == -1)
		throw Http404Exception();
	if (FileHandler::folderExists(fullPath))
		throw Http403Exception();
	if (access(fullPath, W_OK) == -1)
		throw Http403Exception();
	std::remove(fullPath);
	m_res.setStatusCode(204);
	m_res.setBody("File deleted successfully.");
	m_res.setHeader(HEADER_CONTENT_TYPE, MIME_PLAIN);
}


/**
 *
 * @brief Handles HTTP GET requests.
 *
 * This function processes the GET request,
 * retrieves the requested resource,
 * and prepares the HTTP response with the appropriate headers and body.
 *
 * @throws AHttpException if the requested resource is not found
 * or if there are permission issues.
 *
 */
void Client::handleGet(std::string ressourcePath)
{
	const std::string target	= m_req.getTarget();
	const size_t	  targetLen = target.length();


	try
	{
		if (FileHandler::folderExists(ressourcePath))
		{
			const bool isFolderRequest = targetLen && target[targetLen - 1] == '/';

			if (!isFolderRequest)
				throw Http404Exception();
			bool			  foundIndex   = false;
			const std::string fallbackPath = ressourcePath + "/" + FALLBACK_INDEX;
			if (m_serverConfig.hasIndexFiles(target))
			{
				const Vec<std::string> indexFiles = m_serverConfig.getIndexFiles(target);

				for (size_t i = 0; i < indexFiles.size(); ++i)
				{
					const std::string indexPath = String::removeDuplicateSlashes(ressourcePath + "/" + indexFiles[i]);
					if (FileHandler::fileExists(indexPath))
					{
						ressourcePath = indexPath;
						foundIndex	  = true;
						break;
					}
				}
			}
			if (!foundIndex && FileHandler::fileExists(fallbackPath))
				ressourcePath = fallbackPath;
			else if (!foundIndex && m_req.getActiveAutoIndex())
			{
				if (!m_req.matchAccept(MIME_HTML))
					throw Http406Exception();
				IndexOf index(ressourcePath, target);
				m_res.setBody(index.getPage());
				m_res.setHeader(HEADER_CONTENT_TYPE, MIME_HTML);
				m_res.setStatusCode(200);
				return;
			}
		}
		File file(ressourcePath, target);
		if (file.getIsDirectory())
			throw Http404Exception();
		if (!m_req.matchAccept(file.getMime()))
			throw Http406Exception();
		m_res.setBody(file.read().getContent());
		m_res.setHeader(HEADER_CONTENT_TYPE, file.getMime());
		m_res.setHeader(HEADER_LAST_MODIFIED, file.getLastModified());
		m_res.setStatusCode(200);
	}
	catch (const FileHandler::FileHandlerException &e)
	{
		std::cerr << e.what() << '\n';
		throw e.getHttpException();
	}
}

/**
 *
 * @brief Prepares the HTTP response based on the client's request.
 *
 * This function determines the requested method (GET, POST, DELETE),
 * validates it, and calls the appropriate handler to process the request.
 *
 * It also checks if the request is for a CGI script
 * and spawns the CGI handler if necessary.
 *
 *
 * @throws AHttpException for various HTTP errors
 *
 * @return false if a CGI script is being handled, true otherwise.
 *
 */
bool Client::prepareResponse()
{
	CgiSpawner cgiHandler(m_req, m_serverConfig);

	if (!m_req.isAllowedMethod())
		throw Http403Exception();
	if (cgiHandler.isCgi())
	{
		cgiHandler.spawnCgi(this);
		m_events = 0;
		return false;
	}

	const size_t		   segmentSize	 = m_req.getSegmentSize();
	const std::string	   ressourcePath = m_req.buildResourcePath(segmentSize - 1);
	const HttpMethod::Type method		 = m_req.getMethod().getType();
	if (method == HttpMethod::GET)
		handleGet(ressourcePath);
	else if (method == HttpMethod::POST)
		handlePost();
	else if (method == HttpMethod::DELETE)
		handleDelete(ressourcePath);
	else if (method == HttpMethod::PUT || method == HttpMethod::PATCH)
		handlePutPatch(ressourcePath);
	else
		throw Http405Exception(m_req.getMethod().getStrType());
	return true;
}


/**
 *
 * @brief Determines if the connection should be kept alive.
 *
 * This function checks the client's request headers and the response status code
 * to decide whether to keep the connection alive for further requests or to close it.
 *
 * @param statusCode The HTTP status code of the response.
 *
 * @note It stores the result in the m_shouldClose member variable to then be used in
 * the onPollOut() method to decide whether to close the connection after sending the response.
 *
 * @return true if the connection should be kept alive, false if it should be closed.
 *
 */
bool Client::determineKeepAlive(unsigned short statusCode)
{
	m_shouldClose = !m_req.getConnectionKeepAlive() && !m_res.isCloseStatus(statusCode);
	return !m_shouldClose;
}

/**
 *
 * @brief Handles reading data from the client socket.
 *
 * This function reads data from the client's socket,
 * checks if the complete HTTP request * has been received,
 * and prepares the response if so.
 * If the connection is closed by the client,
 * it updates the client state accordingly.
 *
 * @note This function is called when the client socket is ready for reading.
 *
 */
void Client::onPollIn()
{
	const time_t currentTime = std::time(0);
	try
	{
		const ssize_t bytesRead = m_req.readOnce(m_fd);
		if (bytesRead == -1)
		{
			m_events		   = 0;
			m_connectionClosed = true;
			return;
		}
		if (bytesRead == 0)
			return;
		m_time = currentTime;
		if (!m_req.isComplete(m_serverConfig.getMaxBodySize()))
		{
			m_timeoutState = m_req.isHeadersEndPos() ? BODY_TIMEOUT : HEADER_TIMEOUT;
			return;
		}
		m_timeoutState = KEEP_ALIVE_TIMEOUT;
		m_req.parse(m_serverConfig);
		if (!prepareResponse())
			return;
	}
	catch (const AHttpException &e)
	{
		m_res.reset();
		m_res.setStatusCode(e.getStatusCode());
		m_res.setHeader(HEADER_CONTENT_TYPE, MIME_HTML);
		m_res.setBody(e.getPage());
	}
	m_res.fill(!m_req.getConnectionKeepAlive(), m_serverConfig.getKeepAliveTimeout()).build();
	determineKeepAlive(m_res.getStatusCode());
	m_events = POLLOUT;
}




/**
 *
 * @brief Handles writing data to the client socket.
 *
 * This function sends the prepared HTTP response to the client.
 * If the entire response has been sent,
 * it updates the client state to CLOSED and resets the response object.
 *
 * @note This function is called when the client socket is ready for writing.
 *
 */
void Client::onPollOut()
{
	bool hasFullySent = false;
	try
	{
		hasFullySent = m_res.sendOnce(m_fd);
	}
	catch (const std::exception &e)
	{
		m_events		   = 0;
		m_connectionClosed = true;
		return;
	}
	if (hasFullySent)
	{
		if (m_shouldClose)
		{
			m_connectionClosed = true;
			m_events		   = 0;
			return;
		}
		reset();
		m_count++;
		return;
	}
	m_events = POLLOUT;
}


/**
 *
 * @brief Handles errors that occur during polling of the client socket.
 *
 * This function is called when an error event is detected on the client socket.
 * It logs the error details and updates the client state to indicate that the connection is closed.
 *
 * @note This function is called when a POLLERR event is detected on the client socket.
 *
 */
void Client::onPollErr()
{
	Logger::error("Poll error on client socket fd " + Logger::get(m_fd) + ", errno: " + Logger::get(errno) + ", strerror: " + strerror(errno));
	m_events		   = 0;
	m_connectionClosed = true;
}


/**
 *
 * @brief Handles the hang-up event on the client socket.
 *
 * This function is called when a POLLHUP event is detected on the client socket,
 * indicating that the client has disconnected. It logs the disconnection details
 * and updates the client state to indicate that the connection is closed.
 *
 * @note This function is called when a POLLHUP event is detected on the client socket.
 *
 */
void Client::onPollHangUp()
{

	Logger::info("client disconnected on fd " + Logger::get(m_fd) + ", errno: " + Logger::get(errno) + ", strerror: " + strerror(errno));
	m_events		   = 0;
	m_connectionClosed = true;
}



/**
 *
 * @brief Checks if the client connection has timed out.
 *
 * This function calculates the timeout based on the client's current timeout state
 * (keep-alive, header, or body) and the server's configured timeouts.
 * It compares the current time with the calculated timeout time to determine if the connection has timed out.
 *
 * @param currentTime The current time to compare against the timeout time.
 *
 * @return true if the connection has timed out, false otherwise.
 *
 */
bool Client::isTimedOut(const time_t currentTime) const
{
	time_t timeoutTime = m_time;

	if (m_timeoutState == KEEP_ALIVE_TIMEOUT)
		timeoutTime += m_serverConfig.getKeepAliveTimeout();
	if (m_timeoutState == HEADER_TIMEOUT)
		timeoutTime += m_serverConfig.getHeaderTimeout();
	if (m_timeoutState == BODY_TIMEOUT)
		timeoutTime += m_serverConfig.getBodyTimeout();
	return currentTime > timeoutTime;
}

void Client::onTimeOut()
{
	if (m_timeoutState == HEADER_TIMEOUT || m_timeoutState == BODY_TIMEOUT)
	{
		m_res.reset();
		m_res.setStatusCode(408);
		m_res.setHeader(HEADER_CONTENT_TYPE, MIME_HTML);
		m_res.setBody(Http408Exception().getPage());
		m_res.fill(true, 0).build();
		determineKeepAlive(m_res.getStatusCode());
		m_events = POLLOUT;
		return;
	}
	m_connectionClosed = true;
	m_events		   = 0;
}