#include "Client.hpp"
#include "config.hpp"
#include <unistd.h>
#include <cstring>
#include "FileHandler.hpp"
#include "httpExceptions.hpp"
#include "Date.hpp"
#include "IndexOf.hpp"
#include "CgiSpawner.hpp"
#include "CgiPipe.hpp"
#include "EventLoop.hpp"

Client::Client(int fd, VirtualServer *server) :
	Pollable(fd),
	m_server(server)
{
	m_events = POLLIN;
}

Client::~Client()
{
	if (m_fd != -1)
		close(m_fd);
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
	(void) 0;
}


void Client::handleDeleteRequest()
{
	(void) 0;
}



void Client::test(const std::string &ROOT)
{
	(void) m_server;
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
		IndexOf index(ROOT, target);
		m_res.setBody(index.getPage());
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
void Client::handleGetRequest()
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
			resource = ROOT + "/index.html";
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

		m_res.setStatusCode(200);
	}
	catch (const FileHandler::FileHandlerException &e)
	{
		std::cerr << e.what() << '\n';
		throw e.getHttpException();
	}
}


bool Client::isCgi()
{
	try
	{
		File			  file(m_req.getTarget(), m_req.getTarget(), "files/www/website");
		const std::string ext = file.getExtension();

		return ext == ".php" || ext == ".py";
	}
	catch (const FileHandler::FileHandlerException &e)
	{
		std::cerr << e.what() << '\n';
		throw e.getHttpException();
	}
	return false;
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
	const std::string method = m_req.getMethod();
	try
	{
		if (isCgi())
		{
			spawnCgi(m_server->getDocumentRoot() + m_req.getTarget());
			return;
		}
		if (method == "GET")
			handleGetRequest();
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
	m_req.parse();
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
}




/**
 *
 * @brief Spawns a CGI script for the client request.
 *
 * This function creates a CgiSpawner instance to handle the CGI script execution.
 * It sets up the necessary pipes and spawns the CGI process.
 * If successful, it creates a CgiPipe to read the CGI output.
 * If spawning fails, it prepares a 500 Internal Server Error response.
 *
 * @param script_path The path to the CGI script to be executed.
 *
 */
void Client::spawnCgi(const std::string &script_path)
{
	CgiSpawner spawner(this, m_server);
	int		   stdout_fd = spawner.spawn(script_path);

	if (stdout_fd >= 0)
	{
		CgiPipe *cgi_pipe = new CgiPipe(spawner.getPid(),
										stdout_fd, this, script_path);
		EventLoop::addHandler(cgi_pipe);
		m_events = 0;
	}
	else
	{
		m_res.reset();
		m_res.setStatusCode(500);
		m_res.setBody("CGI spawn failed");
		m_res.build();
		m_events = POLLOUT;
	}
}
