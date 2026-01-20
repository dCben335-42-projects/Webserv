#include "CgiSpawner.hpp"
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdlib>
#include "config.hpp"
#include <unistd.h>
#include "File.hpp"

CgiSpawner::CgiSpawner(const HttpRequest &req, const Config &config) :
	m_req(req),
	m_config(config),
	m_pid(-1),
	m_has_post_data(false)
{
	m_has_post_data = !m_req.getBody().empty();
	std::cout << m_has_post_data << std::endl;
}

int CgiSpawner::spawn(const std::string &script_path)
{
	setupPipes();
	setupEnv();
	m_pid = fork();
	if (m_pid == -1)
	{
		if (m_stdout_pipe[1] != -1)
			close(m_stdout_pipe[1]);
		if (m_stdout_pipe[0] != -1)
			close(m_stdout_pipe[0]);
		if (m_stdin_pipe[1] != -1)
			close(m_stdin_pipe[1]);
		if (m_stdin_pipe[0] != -1)
			close(m_stdin_pipe[0]);
		return -1;
	}
	if (m_pid == 0)
		childprocess(script_path);
	parentProcess();
	return m_stdout_pipe[0];
}

void CgiSpawner::setupPipes()
{
	// TODO maybe need to throw ?
	if (pipe(m_stdin_pipe) == -1)
	{
		std::cerr << "Pipe creation failed" << std::endl;
		m_stdin_pipe[0] = -1;
		m_stdin_pipe[1] = -1;
	}
	if (pipe(m_stdout_pipe) == -1)
	{
		std::cerr << "Pipe creation failed" << std::endl;
		m_stdout_pipe[0] = -1;
		m_stdout_pipe[1] = -1;
	}
}

std::string CgiSpawner::makeEnvVar(const std::string &key, const std::string &value)
{
	return key + '=' + value;
}

void CgiSpawner::setupEnv()
{
	m_env.push(makeEnvVar("HTTP_ACCEPT", m_req.getHeader(HEADER_ACCEPT)));
	m_env.push(makeEnvVar("HTTP_ACCEPT_ENCODING", m_req.getHeader(HEADER_ACCEPT_ENCODING)));
	m_env.push(makeEnvVar("HTTP_ACCEPT_LANGUAGE", m_req.getHeader(HEADER_ACCEPT_LANGUAGE)));
	m_env.push(makeEnvVar("HTTP_USER_AGENT", m_req.getHeader(HEADER_USER_AGENT)));
	m_env.push(makeEnvVar("HTTP_COOKIE", m_req.getHeader(HEADER_COOKIE)));

	m_env.push(makeEnvVar("GATEWAY_INTERFACE", GATEWAY_INTERFACE));
	m_env.push(makeEnvVar("REQUEST_METHOD", m_req.getMethod()));
	m_env.push(makeEnvVar("HTTP_HOST", m_req.getHeader(HEADER_HOST)));
	m_env.push(makeEnvVar("SERVER_NAME", PROJECT_NAME));
	m_env.push(makeEnvVar("SCRIPT_NAME", m_req.getTarget()));
	m_env.push(makeEnvVar("SERVER_PROTOCOL", PROTOCOL));
	m_env.push(makeEnvVar("PATH_INFO", "\"\""));

	if (m_has_post_data)
	{
		m_env.push(makeEnvVar("CONTENT_TYPE", m_req.getHeader(HEADER_CONTENT_TYPE)));
		m_env.push(makeEnvVar("CONTENT_LENGTH", m_req.getHeader(HEADER_CONTENT_LENGTH)));
	}
}

#include <stdio.h>
void CgiSpawner::childprocess(const std::string &script_path)
{
	const std::string &fileExt = File::determineExtension(, NULL);
	const std::string &bin	   = m_config.getCgiBin(m_req.getTarget(), );
	char *const		   argv[3] = {
		   const_cast<char *>(bin.c_str()),
		   const_cast<char *>(script_path.c_str()),
		   NULL};
	generateEnvp();
	if (dup2(m_stdin_pipe[0], STDIN_FILENO) == -1)
		throw std::runtime_error("Dup2 failed");
	close(m_stdin_pipe[1]);
	close(m_stdin_pipe[0]);
	if (dup2(m_stdout_pipe[1], STDOUT_FILENO) == -1)
		throw std::runtime_error("Dup2 failed");
	close(m_stdout_pipe[0]);
	close(m_stdout_pipe[1]);
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
	close(m_stdout_pipe[1]);
	const std::string &body = m_req.getBody();
	write(m_stdin_pipe[1], body.data(), body.size());
	close(m_stdin_pipe[0]);
	close(m_stdin_pipe[1]);
}