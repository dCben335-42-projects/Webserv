// CgiPipe.cpp
#include "CgiPipe.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <fcntl.h>

CgiPipe::CgiPipe(int stdout_pipe_fd, Client *client, const std::string &script_path) :
	Pollable(stdout_pipe_fd),
	m_client(client),
	m_pid(-1),
	m_scriptPath(script_path)
{
	m_events = POLLIN;
}

CgiPipe::~CgiPipe()
{
	if (m_pid > 0)
	{
		kill(m_pid, SIGTERM);
		int status;
		waitpid(m_pid, &status, WNOHANG);
	}
	if (m_fd >= 0)
	{
		close(m_fd);
	}
}

void CgiPipe::onPollIn()
{
	char	buffer[8192];
	ssize_t bytes_read = read(m_fd, buffer, sizeof(buffer));

	if (bytes_read > 0)
	{
		HttpResponse &res = m_client->getResponse();
		res.appendBody(buffer, bytes_read);
		return;
	}
	if (bytes_read == 0)
	{
		close(m_fd);
		m_events = 0;
		m_client->setEvents(POLLOUT);
		return;
	}
	onError();
}

void CgiPipe::onError()
{
	close(m_fd);
	m_events		  = 0;
	HttpResponse &res = m_client->getResponse();
	// Send 500 error to client
	res.reset();
	res.setStatusCode(500);
	res.setHeader("Content-Type", "text/html");
	res.setBody("<h1>CGI Script Error</h1>");
	res.build();
	m_client->setEvents(POLLOUT);
}


void CgiPipe::onPollOut()
{
	// Not used for CGI pipe
}


void CgiPipe::onPollErr()
{
	std::cerr << "CgiPipe error on fd " << m_fd << " for script " << m_scriptPath << std::endl;
	onError();
}