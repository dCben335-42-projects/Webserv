#include "CgiPipe.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <fcntl.h>
#include "config.hpp"
#include <errno.h>

CgiPipe::CgiPipe(
	pid_t			   pid,
	int				   stdout_pipe_fd,
	Client			  *client,
	const std::string &script_path) :
	Pollable(stdout_pipe_fd),
	m_client(client),
	m_pid(pid),
	m_scriptPath(script_path)
{
	m_events = POLLIN;
}

CgiPipe::~CgiPipe()
{
	std::cerr << RED "destructor" << RESET << std::endl;
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


#include <dirent.h>
#include <vector>
#include <string>
#include "Logger.hpp"
#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include <poll.h>


std::vector<int> get_open_fds()
{
	std::vector<int> fds;
	int				 max_fd = sysconf(_SC_OPEN_MAX);

	for (int fd = 0; fd < max_fd; ++fd)
	{
		if (fcntl(fd, F_GETFD) != -1 || errno != EBADF)
		{
			fds.push_back(fd);
		}
	}
	log(fds);
	return fds;
}


void CgiPipe::onPollIn()
{
	char	buffer[8192] = {'\0'};
	ssize_t bytes_read	 = read(m_fd, buffer, 8192);

	std::cerr << strerror(errno) << std::endl;
	std::cerr << m_fd << std::endl;
	get_open_fds();
	if (bytes_read > 0)
	{
		HttpResponse &res = m_client->getResponse();
		res.appendToFulFullResponse(buffer, bytes_read);
	}
	if (bytes_read == 0)
	{
		std::cerr << "salope" << std::endl;
		waitChild();
		m_events = 0;
		m_client->setEvents(POLLOUT);
		m_connectionClosed = true;
		return;
	}
	std::cerr << "ICI Ta mere" << std::endl;
	onError();
}

void CgiPipe::onError()
{
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