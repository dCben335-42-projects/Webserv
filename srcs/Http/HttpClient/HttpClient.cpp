#include "HttpClient.hpp"
#include "config.hpp"
#include <unistd.h>
#include <cstring>
#include "FileHandler.hpp"
#include "httpErrors.hpp"

HttpClient::HttpClient(int fd) : m_fd(fd), m_state(READING)
{
}
HttpClient::~HttpClient()
{
}

int HttpClient::getFd() const
{
	return m_fd;
}

ClientState HttpClient::getState() const
{
	return m_state;
}

void HttpClient::setState(ClientState state)
{
	m_state = state;
}

void HttpClient::prepareResponse()
{
	const std::string ROOT			= "files/www/website";
	const std::string FALLBACK_TEST = ROOT + "/index.html";

	try
	{
		try
		{
			if (m_req.getTarget() == "/" && m_req.getHeader("Host") == "localhost:8080")
			{
				m_res.setBody(FileHandler::readFile(FALLBACK_TEST));
				m_res.setHeader(HEADER_CONTENT_TYPE, FileHandler::getContentType(FALLBACK_TEST));
			}
			else
			{
				std::string safePath = FileHandler::getSafePath(ROOT, m_req.getTarget().substr(1));
				m_res.setBody(FileHandler::readFile(safePath));
				m_res.setHeader(HEADER_CONTENT_TYPE, FileHandler::getContentType(safePath));
			}
		}
		catch (const std::exception &e)
		{
			std::cerr << e.what() << '\n';
			throw Http404Error();
		}
		m_res.setStatusCode(200);
	}
	catch (const AHttpError &e)
	{
		m_res.setStatusCode(e.getStatusCode());
		m_res.setHeader(HEADER_CONTENT_TYPE, HEADER_TYPE_JSON);
		m_res.setBody("{\n\t\"status\": \"error\",\n\t\"message\": \"" + std::string(e.what()) + "\"\n}");
	}
	m_res.build();
}

bool HttpClient::shouldClose() const
{
	return m_req.getHeader("Connection") == "close";
}


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
	std::cout << "======> Response " << std::endl;
	std::cout << m_res << std::endl;
	m_state = WRITING;
}

void HttpClient::onWrite(pollfd &pollFd)
{
	if (m_res.sendRes(pollFd.fd))
	{
		m_state		  = CLOSED;
		pollFd.events = 0;
		m_res.reset();
		return;
	}
	pollFd.events |= POLLOUT;
}