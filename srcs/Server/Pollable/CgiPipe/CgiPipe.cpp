#include "CgiPipe.hpp"
#include <sys/wait.h>
#include "webserv.hpp"

CgiPipe::CgiPipe(
	pid_t		  pid,
	int			  stdout_pipe_fd,
	Client *const client) :
	Pollable(stdout_pipe_fd),
	m_client(client),
	m_pid(pid)
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
				std::cout << "Wrong error code " << exit_status << std::endl;
				onError();
			}
		}
		else
		{
			std::cout << "Exited with signal" << std::endl;
			onError();
		}
	}
}


void CgiPipe::onPollIn()
{
	char buffer[READ_BUFFER_SIZE] = {'\0'};
	int	 bytes_read				  = read(m_fd, buffer, READ_BUFFER_SIZE);

	if (bytes_read > 0)
	{
		HttpResponse &res = m_client->getResponse();
		res.appendToFulFullResponse(buffer, bytes_read);
		return;
	}

	if (bytes_read < 0)
		std::cerr << "Read error" << std::endl;
	onError();
}

void CgiPipe::onError()
{
	m_events		  = 0;
	HttpResponse &res = m_client->getResponse();
	res.reset();
	res.setStatusCode(500);
	res.setHeader("Content-Type", "text/html");
	res.setBody("<h1>CGI Script Error</h1>");
	res.build();
	m_client->setEvents(POLLOUT);
}


void CgiPipe::onPollOut()
{
}


void CgiPipe::onPollErr()
{
	std::cerr << "CgiPipe error on fd " << m_fd << std::endl;
	onError();
}

#include "Logger.hpp"
void CgiPipe::onPollHangUp()
{
	std::cerr << "Read ended" << std::endl;
	waitChild();
	m_events = 0;
	m_client->setEvents(POLLOUT);
	m_connectionClosed = true;
	std::cout << "Response: " << m_client->getResponse().getFullResponse() << std::endl;
}