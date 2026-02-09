#include "CgiPipe.hpp"
#include <sys/wait.h>
#include "webserv.hpp"
#include <cstring>
#include "Signals.hpp"
#include "httpMimes.hpp"
#include "CgiOutput.hpp"

CgiPipe::CgiPipe(
	pid_t		  pid,
	int			  stdout_pipe_fd,
	Client *const client) :
	Pollable(stdout_pipe_fd),
	m_client(client),
	m_pid(pid)
{
}

CgiPipe::~CgiPipe()
{
	if (m_pid > 0 && !Signals::getIsChild())
	{
		kill(m_pid, SIGTERM);
		int status;
		waitpid(m_pid, &status, WNOHANG);
	}
	if (m_fd >= 0)
		close(m_fd);
}

void CgiPipe::waitChild()
{
	int	  exit_status;
	int	  status;
	pid_t id;

	id = waitpid(m_pid, &status, WNOHANG);
	if (id > 0)
	{
		if (WIFEXITED(status))
		{
			exit_status = WEXITSTATUS(status);
			if (exit_status == 0)
			{
				m_events = 0;
				m_client->setEvents(POLLOUT);
			}
			else
			{
				std::cerr << "Wrong error code " << exit_status << std::endl;
				onError();
			}
		}
		else
		{
			std::cerr << "Exited with signal" << std::endl;
			onError();
		}
	}
}


void CgiPipe::onPollIn()
{
	char buffer[READ_CGI_BUFFER_SIZE] = {'\0'};
	int	 bytesRead					  = read(m_fd, buffer, READ_CGI_BUFFER_SIZE - 1);

	if (bytesRead > 0)
	{
		m_cgiOutput.appendToCgiOutput(buffer, bytesRead);
		return;
	}
	if (bytesRead < 0)
		std::cerr << "Read error" << std::endl;
	onError();
}

void CgiPipe::onError()
{
	m_events		  = 0;
	HttpResponse &res = m_client->getResponse();
	res.reset();
	res.setStatusCode(500);
	res.setHeader(HEADER_CONTENT_TYPE, MIME_HTML);
	res.setBody("<h1>CGI Script Error</h1>");
	res.build();
	m_client->setEvents(POLLOUT);
}

void CgiPipe::onPollOut()
{
	const std::string &body = m_client->getRequest().getBody();
	write(m_fd, body.data(), body.size());
	m_events		   = 0;
	m_connectionClosed = true;
}

void CgiPipe::onPollErr()
{
	std::cerr << "CgiPipe error on fd " << m_fd << std::endl;
	onError();
}



bool CgiPipe::isTimedOut(const time_t currentTime) const
{
	(void) currentTime;
	return false;
}

#include "Logger.hpp"
void CgiPipe::onPollHangUp()
{
	char	buffer[READ_CGI_BUFFER_SIZE] = {'\0'};
	ssize_t bytesRead					 = read(m_fd, buffer, READ_CGI_BUFFER_SIZE - 1);

	if (bytesRead < 0)
	{
		std::cerr << "Read error" << std::endl;
		return;
	}
	if (bytesRead > 0)
	{
		m_cgiOutput.appendToCgiOutput(buffer, bytesRead);
		return;
	}
	waitChild();
	m_cgiOutput.parse(m_client->getRequest().getConnectionKeepAlive());
	m_cgiOutput.build();
	HttpResponse &res = m_client->getResponse();
	res.setFullResponse(m_cgiOutput.getRes());
	m_events = 0;
	m_client->setEvents(POLLOUT);
	m_connectionClosed = true;
}
