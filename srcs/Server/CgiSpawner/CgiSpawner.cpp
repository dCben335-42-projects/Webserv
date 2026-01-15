#include "CgiSpawner.hpp"
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdlib>

CgiSpawner::CgiSpawner(Client *client, VirtualServer *server) :
	m_client(client),
	m_server(server),
	m_pid(-1),
	m_has_post_data(false)
{
	m_has_post_data = !m_client->getRequest().getBody().empty();
}

int CgiSpawner::spawn(const std::string &script_path)
{
	setupPipes();
	setupEnv(script_path);

	m_pid = fork();
	if (m_pid == -1)
	{
		closeUnusedPipes();
		return -1;
	}
	if (m_pid == 0)
		childprocess(script_path);
	parentProcess();
	return m_stdout_pipe[0];
}

void CgiSpawner::setupPipes()
{
	if (m_has_post_data)
	{
		pipe2(m_stdin_pipe, O_NONBLOCK | O_CLOEXEC);
	}
	pipe2(m_stdout_pipe, O_NONBLOCK | O_CLOEXEC);
}

void CgiSpawner::setupEnv(const std::string &script_path)
{
	// const HttpRequest &req = m_client->getRequest();
	// setenv("REQUEST_METHOD", req.getMethod().c_str(), 1);
	setenv("SCRIPT_NAME", script_path.c_str(), 1);
	// setenv("QUERY_STRING", m_client->m_req.getQueryString().c_str(), 1);
	// setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
	// setenv("SERVER_SOFTWARE", "Webserv/1.0", 1);

	// if (m_has_post_data)
	// {
	// 	setenv("CONTENT_LENGTH", std::to_string(m_client->m_req.getBody().size()).c_str(), 1);
	// 	setenv("CONTENT_TYPE", m_client->m_req.getHeader("Content-Type").c_str(), 1);
	// }
}

void CgiSpawner::childprocess(const std::string &script_path)
{
	(void) script_path;
	// Configure stdin (POST data)
	// if (m_has_post_data)
	// {
	// 	close(m_stdin_pipe[0]);
	// 	dup2(m_stdin_pipe[1], STDIN_FILENO);
	// }
	// else
	// {
	// 	close(STDIN_FILENO); // No input
	// }

	// // Configure stdout + stderr
	// close(m_stdout_pipe[0]);
	// dup2(m_stdout_pipe[1], STDOUT_FILENO);
	// dup2(m_stdout_pipe[1], STDERR_FILENO);

	// closeUnusedPipes();

	// Exec script via /bin/sh (portable .py, .pl, .sh)
	// std::string full_path = m_server->getCgiBin() + script_path;
	// execl("/bin/sh", "sh", "-c", full_path.c_str(), (char *) NULL);

	// Fallback si exec échoue
	_exit(127);
}

void CgiSpawner::parentProcess()
{
	closeUnusedPipes();
	if (m_has_post_data)
	{
		writePostData();
	}
}

void CgiSpawner::writePostData()
{
	const std::string &body = m_client->getRequest().getBody();
	close(m_stdin_pipe[0]);
	ssize_t bytes_written = write(m_stdin_pipe[1],
								  body.data(),
								  body.size());
	(void) bytes_written;
	(void) m_server;
	close(m_stdin_pipe[1]);
}

void CgiSpawner::closeUnusedPipes()
{
	if (m_has_post_data)
	{
		if (m_pid != 0)
			close(m_stdin_pipe[0]);
		if (m_pid == 0)
			close(m_stdin_pipe[1]);
	}
	if (m_pid != 0)
		close(m_stdout_pipe[1]);
	if (m_pid == 0)
		close(m_stdout_pipe[0]);
}
