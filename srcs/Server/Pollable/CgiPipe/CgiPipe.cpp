#include "CgiPipe.hpp"
#include <sys/wait.h>
#include "webserv.hpp"
#include <cstring>
#include "Signals.hpp"
#include "httpMimes.hpp"
#include "CgiOutput.hpp"
#include "httpExceptions.hpp"
#include "Logger.hpp"
#include "log.hpp"
#include "Date.hpp"


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


/**
 *
 * @brief CgiPipe destructor, if called inside the parent process, kill the child
 * and wait for the kill() call to take effect
 *
 */
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

/**
 *
 * @brief Waits for the child to end, without blocking, and reacts differently
 * depending on the exit status of the child process
 *
 * @throw Http500Exception() if the child exited unexpectedly
 *
 */
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


/**
 *
 * @brief Function to handle reaction when the poll() call on the CgiPipe
 * returns a POLLOUT event
 *
 * @throw Http500Exception() if write() fails
 *
 */
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


/**
 *
 * @brief Function to handle reaction when the poll() call on the CgiPipe
 * returns a POLLERR event
 *
 */
void CgiPipe::onPollErr()
{
	m_client->onHttpException(Http500Exception());
	m_events = 0;
	m_client->setEvents(POLLOUT);
	m_connectionClosed = true;
	// TODO Logger::openFile(m_errorLog).custom(LOG_MSG_CGIPIPE_PREFIX ERROR_MSG_UNKNOWN);
}

void CgiPipe::onPollHangUp()
{
	char	buffer[READ_CGI_BUFFER_SIZE] = {'\0'};
	ssize_t bytesRead					 = read(m_fd, buffer, READ_CGI_BUFFER_SIZE - 1);

	// TODO Logger::openClient(m_accessLog).custom(LOG_MSG_PIPE_CLOSED);
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

		const HttpRequest &req		  = m_client->getRequest();
		HttpResponse	  &res		  = m_client->getResponse();
		unsigned int	   statusCode = m_cgiOutput.getStatusCode();

		res.setStatusCode(statusCode);
		res.setFullResponse(m_cgiOutput.getRes());
		const std::string logStr = LOG_MSG_REPONSE_READY LOG_GLOBAL_SEPARATOR + req.getMethod().getStrType() + LOG_GLOBAL_SEPARATOR + req.getTarget() + LOG_GLOBAL_SEPARATOR + PROTOCOL + LOG_GLOBAL_SEPARATOR + Number::toString(statusCode);
		if (Logger::openFile(m_accessLog))
			Logger::custom(logStr);
		Logger::setTarget(Logger::Stdout).custom(logStr);
	}
	catch (const AHttpException &e)
	{
		m_client->onHttpException(e);
		m_client->buildResponse();
	}
	m_events = 0;
	m_client->setEvents(POLLOUT);
	m_connectionClosed = true;
}

/**
 *
 * @brief Util function to detect if the Cgi timed out
 *
 */
bool CgiPipe::isTimedOut(const time_t currentTime) const
{
	const time_t cgiTimeOut	  = m_client->getConfig().getCgiTimeout();
	const time_t cgiStartTime = m_client->getTime();

	return Date::getDiffTime(currentTime, cgiStartTime) > cgiTimeOut;
}


/**
 *
 * @brief Function to handle reaction when the Cgi Time out
 *
 * @note It prepares a 504 Gateway Timeout response and updates the client state to indicate that the connection is closed.
 * @note This function is called when a timeout is detected for the CgiPipe.
 *
 */
void CgiPipe::onTimeOut()
{
	m_client->onHttpException(Http504Exception());
	m_client->buildResponse();
	m_client->setEvents(POLLOUT);
	m_events		   = 0;
	m_connectionClosed = true;
	// TODO Logger::openFile(m_errorLog).custom(ERROR_MSG_CGI_TIMEOUT);
}