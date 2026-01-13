#include "HttpClient.hpp"
#include "config.hpp"
#include <unistd.h>
#include <cstring>
#include "FileHandler.hpp"
#include "httpExceptions.hpp"
#include "Date.hpp"

HttpClient::HttpClient(int fd) : m_fd(fd), m_state(READING)
{
}
HttpClient::~HttpClient()
{
}

/**
 *
 * @brief Gets the file descriptor associated with the HTTP client.
 *
 * @return The file descriptor of the client.
 *
 */
int HttpClient::getFd() const
{
	return m_fd;
}

/**
 *
 * @brief Gets the current state of the HTTP client.
 *
 * @see ClientState enum for possible states.
 *
 * @return The current state of the client.
 *
 */
ClientState HttpClient::getState() const
{
	return m_state;
}

/**
 *
 * @brief Sets the state of the HTTP client.
 *
 * @param state The new state to be set for the client.
 *
 * @see ClientState enum for possible states.
 *
 */
void HttpClient::setState(ClientState state)
{
	m_state = state;
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
void HttpClient::handlePostRequest()
{
}



#include "IndexOf.hpp"
std::string testIndexOf(const std::string &target, const std::string &root)
{
	IndexOf index(root, target);
	return index.getPage();
}

void HttpClient::test(const std::string &ROOT)
{
	const std::string target	 = String::removeDuplicateSlashes(m_req.getTarget());
	const std::string targetPath = String::removeDuplicateSlashes(ROOT + target);
	if (FileHandler::fileExists(targetPath))
	{
		m_res.setBody(FileHandler::readFile(targetPath));
		m_res.setHeader(HEADER_CONTENT_TYPE, FileHandler::getContentType(targetPath));
		std::string date = Date::getCurrentDate();
		if (!date.empty())
			m_res.setHeader(HEADER_DATE, date);
		m_res.setStatusCode(200);
		return;
	}
	if (FileHandler::folderExists(targetPath))
	{
		m_res.setBody(testIndexOf(target, ROOT));
		m_res.setHeader(HEADER_CONTENT_TYPE, HEADER_TYPE_HTML);
		std::string date = Date::getCurrentDate();
		if (!date.empty())
			m_res.setHeader(HEADER_DATE, date);
		m_res.setStatusCode(200);
		return;
	}
	else
		throw Http404Exception();
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
void HttpClient::handleGetRequest()
{
	const std::string ROOT			= "files/www/website";
	const std::string FALLBACK_TEST = ROOT + "/index.html";
	std::string		  resource;
	std::string		  target;

	try
	{
		if (m_req.getHeader("Host") == "localhost:9090")
		{
			test(ROOT);
			return;
		}
		if (m_req.getTarget() == "/")
		{
			if (m_req.getHeader("Host") == "localhost:8080")
				resource = FALLBACK_TEST;
			else
				resource = ROOT + "/index.html";
		}
		else
		{
			target	 = m_req.getTarget();
			target	 = target.at(0) == '/' ? target.substr(1) : target;
			resource = FileHandler::getSafePath(ROOT, target);
		}
		m_res.setBody(FileHandler::readFile(resource));
		m_res.setHeader(HEADER_CONTENT_TYPE, FileHandler::getContentType(resource));
		std::string fileDate = FileHandler::findLastModifiedDate(resource);
		if (!fileDate.empty())
			m_res.setHeader(HEADER_LAST_MODIFIED, fileDate);
		std::string date = Date::getCurrentDate();
		if (!date.empty())
			m_res.setHeader(HEADER_DATE, date);
	}
	catch (const FileHandler::FileHandlerException &e)
	{
		std::cerr << e.what() << '\n';
		throw e.getHttpException();
	}
	m_res.setStatusCode(200);
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
void HttpClient::prepareResponse()
{
	try
	{
		if (m_req.getMethod() == "GET")
			handleGetRequest();
		else
			handleGetRequest();
	}
	catch (const AHttpException &e)
	{
		m_res.reset();
		m_res.setStatusCode(e.getStatusCode());
		m_res.setHeader(HEADER_CONTENT_TYPE, HEADER_TYPE_HTML);
		m_res.setBody(e.getPage());
	}
	m_res.build();
}

/**
 *
 * @brief Determines if the HTTP connection should be closed based on the client's request.
 *
 * @return true if the connection should be closed, false otherwise.
 *
 */
bool HttpClient::shouldClose() const
{
	return m_req.getHeader("Connection") == "close";
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
void HttpClient::onRead()
{
	ssize_t n = m_req.readOnce(m_fd);
	if (n == 0)
	{
		m_state = CLOSED;
		return;
	}
	size_t headersEnd = m_req.getRawRequest().find(HEADER_ENDING);
	if (headersEnd == std::string::npos)
		return;
	size_t contentLength = m_req.readContentLength(m_req.getRawRequest(), headersEnd);
	if (m_req.getRawRequest().size() < headersEnd + std::strlen(HEADER_ENDING) + contentLength)
		return;
	m_req.parse();
	prepareResponse();
	m_state = WRITING;
}

void HttpClient::onWrite(pollfd &pollFd)
{
	if (m_res.sendOnce(pollFd.fd))
	{
		m_state		  = CLOSED;
		pollFd.events = 0;
		m_res.reset();
		return;
	}
	pollFd.events |= POLLOUT;
}