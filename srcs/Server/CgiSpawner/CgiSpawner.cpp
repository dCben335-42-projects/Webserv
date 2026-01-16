#include "CgiSpawner.hpp"
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdlib>
#include "config.hpp"
#include <unistd.h>

char body[20] = "My name is Benoit";

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
	setupEnv();
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
	// return m_sockPair[0];
}

void CgiSpawner::setupPipes()
{
	// if (socketpair(AF_UNIX, SOCK_STREAM, 0, m_sockPair) != 0)
	// 	std::cerr << "Socketpair error :" << std::endl;
	// if (m_has_post_data)
	// {
	if (pipe(m_stdin_pipe) == -1)
		std::cerr << "Pipe creation failed" << std::endl;
	// }
	if (pipe(m_stdout_pipe) == -1)
		std::cerr << "Pipe creation failed" << std::endl;
}

std::string CgiSpawner::makeEnvVar(const std::string &key, const std::string &value)
{
	return key + '=' + value;
}

void CgiSpawner::setupEnv()
{
	const HttpRequest &req = m_client->getRequest();

	m_env.push(makeEnvVar("HTTP_ACCEPT", req.getHeader(HEADER_ACCEPT)));
	m_env.push(makeEnvVar("HTTP_ACCEPT_ENCODING", req.getHeader(HEADER_ACCEPT_ENCODING)));
	m_env.push(makeEnvVar("HTTP_ACCEPT_LANGUAGE", req.getHeader(HEADER_ACCEPT_LANGUAGE)));
	m_env.push(makeEnvVar("HTTP_USER_AGENT", req.getHeader(HEADER_USER_AGENT)));
	m_env.push(makeEnvVar("HTTP_COOKIE", req.getHeader(HEADER_COOKIE)));

	m_env.push(makeEnvVar("GATEWAY_INTERFACE", GATEWAY_INTERFACE));
	m_env.push(makeEnvVar("REQUEST_METHOD", req.getMethod()));
	m_env.push(makeEnvVar("HTTP_HOST", req.getHeader(HEADER_HOST)));
	m_env.push(makeEnvVar("SERVER_NAME", PROJECT_NAME));
	m_env.push(makeEnvVar("SCRIPT_NAME", req.getTarget()));
	m_env.push(makeEnvVar("SERVER_PROTOCOL", PROTOCOL));
	m_env.push(makeEnvVar("PATH_INFO", "\"\""));

	m_env.push(makeEnvVar("CONTENT_TYPE", ""));
	m_env.push(makeEnvVar("CONTENT_LENGTH", "0"));

	// if (m_has_post_data)
	// {
	// 		m_env.push(makeEnvVar("CONTENT_TYPE", req.getHeader(HEADER_CONTENT_TYPE)));
	// 		m_env.push(makeEnvVar("CONTENT_LENGTH", req.getHeader(HEADER_CONTENT_LENGTH)));
	// }
}


#include <stdio.h>
#include "Logger.hpp"
void CgiSpawner::childprocess(const std::string &script_path)
{
	const std::string bin	  = m_server->getCgiBin();
	char *const		  argv[3] = {
		  const_cast<char *>(bin.c_str()),
		  const_cast<char *>(script_path.c_str()),
		  NULL};
	generateEnvp();


	if (dup2(m_stdin_pipe[0], STDIN_FILENO) == -1)
		throw std::runtime_error("Dup2 failed");
	if (dup2(m_stdout_pipe[1], STDOUT_FILENO) == -1)
		throw std::runtime_error("Dup2 failed");
	close(m_stdout_pipe[0]);
	close(m_stdout_pipe[1]);
	close(m_stdin_pipe[0]);
	close(m_stdin_pipe[1]);


	// Socketpair part
	// close(m_sockPair[0]);
	// if (dup2(m_sockPair[1], STDIN_FILENO) == -1)
	// 	std::cerr << "Dup2 failed: " << std::endl;
	// if (dup2(m_sockPair[1], STDOUT_FILENO) == -1)
	// 	std::cerr << "Dup2 failed: " << std::endl;
	// close(m_sockPair[1]);


	execve(argv[0], argv, m_envp);
	perror("execve: ");
	exit(127);
}

void CgiSpawner::generateEnvp()
{
	size_t i;
	for (i = 0; i < m_env.size(); i++)
		m_envp[i] = const_cast<char *>(m_env[i].c_str());
	m_envp[i] = NULL;
}

void CgiSpawner::parentProcess()
{
	// write(m_sockPair[0], body, std::strlen(body));
	// close(m_sockPair[1]);
	// write(m_stdin_pipe[1], body, std::strlen(body));
	close(m_stdin_pipe[1]);
	close(m_stdin_pipe[0]);
	close(m_stdout_pipe[1]);
	// closeUnusedPipes();
	// if (m_has_post_data)
	// {
	// writePostData();
	// }
}

void CgiSpawner::writePostData()
{
	// const std::string &body = m_client->getRequest().getBody();
	// close(m_stdin_pipe[0]);
	// ssize_t bytes_written = write(m_stdin_pipe[1],
	//   body.data(),
	//   body.size());
	// (void) bytes_written;
	// (void) m_server;
	// close(m_stdin_pipe[1]);
}

void CgiSpawner::closeUnusedPipes()
{
	// if (m_has_post_data)
	// {
	// 	if (m_pid != 0)
	// 		close(m_stdin_pipe[0]);
	// }
	// if (m_pid != 0)
	// 	close(m_stdout_pipe[1]);
}
