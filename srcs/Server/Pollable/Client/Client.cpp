#include "Client.hpp"
#include "config.hpp"
#include "FileHandler.hpp"
#include "httpExceptions.hpp"
#include "Date.hpp"
#include "IndexOf.hpp"
#include "CgiSpawner.hpp"
#include "CgiPipe.hpp"
#include "EventLoop.hpp"
#include "Location.hpp"
#include "Logger.hpp"
#include "Vec.hpp"
#include "CgiParams.hpp"
#include "Config.hpp"
#include "FileUpload.hpp"

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
	(void) 0;
}

#include "Logger.hpp"

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
void Client::handleGetRequest(const std::string &ressourcePath)
{
	Logger::log("Resource Path: " + ressourcePath);
	Logger::log("Req target: " + m_req.getTarget());

	try
	{
		if (FileHandler::folderExists(ressourcePath))
		{
			IndexOf index(ressourcePath, m_req.getTarget());
			m_res.setBody(index.getPage());
			m_res.setHeader(HEADER_CONTENT_TYPE, HEADER_TYPE_HTML);
			const std::string date = Date::getCurrentDate();
			if (!date.empty())
				m_res.setHeader(HEADER_DATE, date);
			m_res.setStatusCode(200);
			return;
			// if (m_serverConfig.getAutoIndex(m_req.getTarget()))
			// {
			// }
			// {
			// 	if (!FileHandler::fileExists(ressourcePath))
			// 	{
			// 		IndexOf index(root, target);
			// 		m_res.setBody(index.getPage());
			// 		m_res.setHeader(HEADER_CONTENT_TYPE, HEADER_TYPE_HTML);
			// 		std::string date = Date::getCurrentDate();
			// 		if (!date.empty())
			// 			m_res.setHeader(HEADER_DATE, date);
			// 		m_res.setStatusCode(200);
			// 		return;
			// 	}
			// 	resource = root + "/" + m_serverConfig.getIndexFile(target);
			// 	m_res.setBody(FileHandler::readFile(ressourcePath));
			// 	m_res.setHeader(HEADER_CONTENT_TYPE, FileHandler::getContentType(ressourcePath));
			// 	std::string fileDate = FileHandler::findLastModifiedDate(ressourcePath);
			// 	if (!fileDate.empty())
			// 		m_res.setHeader(HEADER_LAST_MODIFIED, fileDate);
			// 	std::string date = Date::getCurrentDate();
			// 	if (!date.empty())
			// 		m_res.setHeader(HEADER_DATE, date);
			// 	m_res.setStatusCode(200);
			// 	return;
			throw Http403Exception();
		}
		m_res.setBody(FileHandler::readFile(ressourcePath));
		m_res.setHeader(HEADER_CONTENT_TYPE, FileHandler::getContentType(ressourcePath));
		const std::string fileDate = FileHandler::findLastModifiedDate(ressourcePath);
		if (!fileDate.empty())
			m_res.setHeader(HEADER_LAST_MODIFIED, fileDate);
		const std::string date = Date::getCurrentDate();
		if (!date.empty())
			m_res.setHeader(HEADER_DATE, date);
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
	const std::string method	  = m_req.getMethod();
	const size_t	  segmentSize = m_req.getSegmentSize();
	try
	{
		if (cgiHandler.isCgi())
		{
			cgiHandler.spawnCgi(this);
			m_events = 0;
			return;
		}
		const std::string ressourcePath = m_req.buildResourcePath(segmentSize - 1);
		if (method == "GET")
			handleGetRequest(ressourcePath);
		else if (method == "POST")
			handlePostRequest();
		else if (method == "DELETE")
			handleDeleteRequest();
		else
			throw Http501Exception(method);
	}
	catch (const AHttpException &e)
	{
		m_res.reset();
		m_res.setStatusCode(e.getStatusCode());
		m_res.setHeader(HEADER_CONTENT_TYPE, HEADER_TYPE_HTML);
		m_res.setBody(e.getPage());
	}
	m_res.build();
	m_events = POLLOUT;
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
	const size_t contentLength = m_req.readContentLength(rawRequest, headersEnd);
	const size_t totalLength   = headersEnd + std::strlen(HEADER_ENDING) + contentLength;

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
	if (!m_req.readOnce(m_fd))
	{
		m_events = 0;
		return;
	}
	if (!isRequestComplete())
	{
		m_events = POLLIN;
		return;
	}
	m_req.parse(m_serverConfig);
	prepareResponse();
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
		m_events		   = 0;
		m_connectionClosed = true;
		return;
	}
	m_events = POLLOUT;
}


void Client::onPollErr()
{
	std::cerr << "Client error on fd " << m_fd << std::endl;
	m_events = 0;
}

void Client::onPollHangUp()
{
	m_connectionClosed = true;
}
