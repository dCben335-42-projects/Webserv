#include "CgiPipe.hpp"
#include <sys/wait.h>
#include "webserv.hpp"
#include <cstring>
#include "Signals.hpp"
#include "httpMimes.hpp"
#include "CgiOutput.hpp"
#include "httpExceptions.hpp"
#include "Logger.hpp"

CgiPipe::CgiPipe(
	pid_t		  pid,
	int			  stdout_pipe_fd,
	Client *const client) :
	Pollable(stdout_pipe_fd),
	m_client(client),
	m_pid(pid),
	m_bytesWritten(0)
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
				throw Http500Exception();
		}
		else
			throw Http500Exception();
	}
}

void CgiPipe::onPollIn()
{
	onPollHangUp();
}

void CgiPipe::onPollOut()
{
	const std::string &body			= m_client->getRequest().getBody();
	const size_t	   remaining	= body.size() - m_bytesWritten;
	const size_t	   to_send		= remaining < WRITE_CLIENT_BUFFER_SIZE ? remaining : WRITE_CLIENT_BUFFER_SIZE;
	ssize_t			   bytesWritten = write(m_fd, body.data() + m_bytesWritten, to_send);

	try
	{
		if (bytesWritten == -1)
			throw Http500Exception();
		m_bytesWritten += bytesWritten;
		if (m_bytesWritten < body.size())
			return;
	}
	catch (const AHttpException &e)
	{
		m_client->onHttpException(e);
	}
	m_events		   = 0;
	m_connectionClosed = true;
}

void CgiPipe::onPollErr()
{
	m_client->onHttpException(Http500Exception());
	m_events = 0;
	m_client->setEvents(POLLOUT);
	m_connectionClosed = true;
	// TODO Logger::openFile(errorFile).custom(LOG_MSG_CGIPIPE_PREFIX ERROR_MSG_UNKNOWN);
}

void CgiPipe::onPollHangUp()
{
	char	buffer[READ_CGI_BUFFER_SIZE] = {'\0'};
	ssize_t bytesRead					 = read(m_fd, buffer, READ_CGI_BUFFER_SIZE - 1);

	// TODO Logger::openClient(accessFile).custom(LOG_MSG_PIPE_CLOSED);
	if (bytesRead > 0)
	{
		m_cgiOutput.appendToCgiOutput(buffer, bytesRead);
		return;
	}
	try
	{
		if (bytesRead < 0)
			throw Http500Exception();
		waitChild();
		m_cgiOutput.parse(m_client->getRequest().getConnectionKeepAlive());
		m_cgiOutput.build();
		HttpResponse &res = m_client->getResponse();
		res.setStatusCode(m_cgiOutput.getStatusCode());
		res.setFullResponse(m_cgiOutput.getRes());
	}
	catch (const AHttpException &e)
	{
		m_client->onHttpException(e);
	}
	m_events = 0;
	m_client->setEvents(POLLOUT);
	m_connectionClosed = true;
}

// TODO CGI timeout
bool CgiPipe::isTimedOut(const time_t currentTime) const
{
	(void) currentTime;
	return false;
}

void CgiPipe::onTimeOut()
{
	HttpResponse &res = m_client->getResponse();
	res.reset();
	res.setStatusCode(504);
	res.setHeader(HEADER_CONTENT_TYPE, MIME_HTML);
	res.setBody("<h1>CGI Script Timeout</h1>");
	res.fill(true, 0).build();
	m_client->setEvents(POLLOUT);
	m_connectionClosed = true;
	// TODO Logger::openFile(errorFile).custom(ERROR_MSG_CGI_TIMEOUT);
}