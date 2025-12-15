#include "HttpClient.hpp"
#include "config.hpp"
#include <unistd.h>
#include <cstring>
#include "FileHandler.hpp"
#include "httpErrors.hpp"
#include "Date.hpp"

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
	std::string		  resource;
	std::string		  target;

	try
	{
		try
		{
			if (m_req.getTarget() == "/" && m_req.getHeader("Host") == "localhost:8080")
				resource = FALLBACK_TEST;
			else
			{
				target	 = m_req.getTarget();
				target	 = target.at(0) == '/' ? target.substr(1) : target;
				resource = FileHandler::getSafePath(ROOT, target);
			}
			m_res.setBody(FileHandler::readFile(resource));
			m_res.setHeader(HEADER_CONTENT_TYPE, FileHandler::getContentType(resource));

			{
				std::string fileDate = FileHandler::findLastModifiedDate(resource);
				if (!fileDate.empty())
					m_res.setHeader(HEADER_LAST_MODIFIED, fileDate);
			}
			{
				std::string date = Date::getCurrentDate();
				if (!date.empty())
					m_res.setHeader(HEADER_DATE, date);
			}
		}
		catch (const std::exception &e)
		{
			std::cerr << e.what() << '\n';
			throw Http404Exception();
		}
		m_res.setStatusCode(200);
	}
	catch (const AHttpException &e)
	{
		m_res.setStatusCode(e.getStatusCode());
		m_res.setHeader(HEADER_CONTENT_TYPE, HEADER_TYPE_JSON);
		m_res.setBody("{\n\t\"status\": \"error\",\n\t\"message\": \"" + std::string(e.what()) + "\"\n}");
	}
	std::cout << FileHandler::findLastModifiedDate(FALLBACK_TEST) << std::endl;
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