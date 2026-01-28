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
	if (!file.isUploadValid())
		throw Http400Exception();
}

void Client::handleDeleteRequest()
{
	m_res.setStatusCode(204);
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
	const std::string target = m_req.getTarget();

	try
	{
		if (FileHandler::folderExists(ressourcePath))
		{
			if (m_req.getActiveAutoIndex())
			{
				IndexOf index(ressourcePath, target);
				m_res.setBody(index.getPage());
				m_res.setHeader(HEADER_CONTENT_TYPE, MIME_HTML);
				m_res.setStatusCode(200);
				return;
			}
			const std::string indexFile = m_serverConfig.getIndexFile(target);
			if (indexFile.empty())
				throw Http403Exception();
			ressourcePath = ressourcePath + "/" + indexFile;
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
 * This function determines the requested resource, reads the corresponding file,
 * and sets up the HTTP response with appropriate headers and body.
 *
 * @note If the requested resource is not found or an error occurs,
 * the response will contain an error page.
 *
 * @see FileHandler for file operations and error handling.
 *
 */
void Client::prepareResponse()
{
	CgiSpawner		  cgiHandler(m_req, m_serverConfig);
	const size_t	  segmentSize	= m_req.getSegmentSize();
	const std::string ressourcePath = m_req.buildResourcePath(segmentSize - 1);

	if (!m_req.isAllowedMethod())
		throw Http403Exception();
	if (cgiHandler.isCgi())
	{
		cgiHandler.spawnCgi(this);
		m_events = 0;
		return;
	}
	const HttpMethod::Type method = m_req.getMethod().getType();
	if (method == HttpMethod::GET)
		handleGetRequest(ressourcePath);
	else if (method == HttpMethod::POST)
		handlePostRequest();
	else if (method == HttpMethod::DELETE)
		handleDeleteRequest();
	else
		throw Http405Exception(m_req.getMethod().getStrType());
}

/**
 *
 * @brief Checks if the HTTP request from the client is complete.
 *
 * This function verifies if the entire HTTP request has been received
 * by checking for the end of headers and the content length.
 *
 * @return true if the request is complete, false otherwise.
 *
 */
bool Client::isRequestComplete()
{
	const std::string rawRequest = m_req.getRawRequest();
	size_t			  headersEnd = rawRequest.find(HEADER_ENDING);
	if (headersEnd == std::string::npos)
		return false;
	m_req.parseFirstLine(rawRequest);
	const ssize_t contentLength = m_req.readContentLength(rawRequest, headersEnd);
	if (contentLength == -1)
		return true;
	const size_t totalLength = headersEnd + std::strlen(HEADER_ENDING) + contentLength;

	if (rawRequest.size() < totalLength)
		return false;
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
	if (!isRequestComplete())
	{
		m_events = POLLIN;
		return;
	}

	try
	{
		m_req.parse(m_serverConfig);
		Logger::error("Received complete request:\n" + m_req.getRawRequest());
		prepareResponse();
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
	const std::string connectionHeader = m_req.getHeader(HEADER_CONNECTION);
	if (connectionHeader == HEADER_CONNECTION_KEEP_ALIVE)
		m_res.setHeader(HEADER_CONNECTION, connectionHeader == HEADER_CONNECTION_KEEP_ALIVE ? HEADER_CONNECTION_KEEP_ALIVE : HEADER_CONNECTION_CLOSE);
	m_res.build();
	m_events = POLLOUT;
}

bool Client::shouldCloseConnection() const
{
	const Map<std::string>			&headers = m_req.getHeaders();
	Map<std::string>::const_iterator it		 = headers.find(HEADER_CONNECTION);
	if (it != headers.end())
	{
		if (it->second == "close")
			return true;
		else if (it->second == "keep-alive")
			return false;
	}
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
		Logger::info("Response sent to client. Waiting for new request.");
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
