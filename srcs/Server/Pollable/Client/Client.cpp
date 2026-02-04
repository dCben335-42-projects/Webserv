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
	m_events = POLLIN;
	EventLoop::addHandler(this);
}

Client::~Client()
{
	if (m_fd != -1)
		close(m_fd);
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
void Client::handlePostRequest()
{
	FileUpload file(m_req);

	if (!m_req.hasUpload())
		throw Http401Exception();
	if (!file.isUploadValid())
		throw Http400Exception();
	m_req.setBody("File uploaded successfully.");
	m_res.setHeader(HEADER_CONTENT_TYPE, MIME_PLAIN);
	m_res.setStatusCode(201);
}

void Client::handleDeleteRequest(const std::string &ressourcePath)
{
	const char *fullPath = ressourcePath.c_str();
	if (access(fullPath, F_OK) == -1)
		throw Http404Exception();
	if (access(fullPath, R_OK | W_OK) == -1)
		throw Http403Exception();
	std::remove(fullPath);
	m_res.setStatusCode(204);
	m_res.setBody("File deleted successfully.");
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
void Client::handleGetRequest(std::string ressourcePath)
{
	const std::string target		  = m_req.getTarget();
	const size_t	  targetLen		  = target.length();
	bool			  isFolderRequest = targetLen && target[targetLen - 1] == '/';

	try
	{
		if (isFolderRequest && FileHandler::folderExists(ressourcePath))
		{
			const std::string fallbackPath = ressourcePath + "/" + FALLBACK_INDEX;
			if (m_serverConfig.hasIndexFile(target))
			{
				const std::string indexPath = ressourcePath + "/" + m_serverConfig.getIndexFile(target);
				if (!FileHandler::fileExists(indexPath))
					throw Http403Exception();
				ressourcePath = indexPath;
			}
			else if (FileHandler::fileExists(fallbackPath))
				ressourcePath = fallbackPath;
			else if (m_req.getActiveAutoIndex())
			{
				IndexOf index(ressourcePath, target);
				m_res.setBody(index.getPage());
				m_res.setHeader(HEADER_CONTENT_TYPE, MIME_HTML);
				m_res.setStatusCode(200);
				return;
			}
		}
		m_res.setBody(FileHandler::readFile(ressourcePath));
		m_res.setHeader(HEADER_CONTENT_TYPE, FileHandler::getContentType(ressourcePath));
		const std::string fileDate = FileHandler::findLastModifiedDate(ressourcePath);
		if (!fileDate.empty())
			m_res.setHeader(HEADER_LAST_MODIFIED, fileDate);
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
		handleGetRequest(ressourcePath);
	else if (method == HttpMethod::POST)
		handlePostRequest();
	else if (method == HttpMethod::DELETE)
		handleDeleteRequest(ressourcePath);
	else
		throw Http405Exception(m_req.getMethod().getStrType());
	return true;
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
	if (m_req.readOnce(m_fd) == -1)
	{
		m_events = 0;
		return;
	}
	try
	{
		if (!m_req.isComplete())
			return;
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
	const std::string date = Date::getCurrentHttpDate();
	if (!date.empty())
		m_res.setHeader(HEADER_DATE, date);
	m_res.setHeader(HEADER_CONNECTION, shouldCloseConnection() ? HEADER_CONNECTION_CLOSE : HEADER_CONNECTION_KEEP_ALIVE);
	m_res.build();
	m_events = POLLOUT;
}


/**
 *
 * @brief Determines if the connection should be closed after the response.
 *
 * This function checks the "Connection" header in the HTTP request
 * to decide whether to keep the connection alive or close it.
 *
 * @return true if the connection should be closed, false otherwise.
 *
 */
bool Client::shouldCloseConnection() const
{
	const std::string connectionHeader = m_req.getHeader(HEADER_CONNECTION);

	if (connectionHeader == HEADER_CONNECTION_KEEP_ALIVE)
		return false;
	if (connectionHeader == HEADER_CONNECTION_CLOSE)
		return true;
	return true;
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
	if (m_res.sendOnce(m_fd))
	{
		if (shouldCloseConnection())
		{
			m_connectionClosed = true;
			m_events		   = 0;
			return;
		}
		m_req.reset();
		m_res.reset();
		m_events = POLLIN;
		return;
	}
	m_events = POLLOUT;
}

void Client::onPollErr()
{
	Logger::error("Poll error on client socket.");
	m_events = 0;
}

void Client::onPollHangUp()
{
	Logger::info("Client disconnected.");
	m_connectionClosed = true;
	m_events		   = 0;
}
