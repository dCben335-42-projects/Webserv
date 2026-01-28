#include "CgiSpawner.hpp"

#include "webserv.hpp"
#include "FileHandler.hpp"
#include "EventLoop.hpp"
#include "CgiPipe.hpp"

#include "Location.hpp"
#include "CgiParams.hpp"

#include "Logger.hpp"

CgiSpawner::CgiSpawner(const HttpRequest &req, const Config &config) :
	m_req(req),
	m_config(config),
	m_pid(-1),
	m_has_post_data(false)
{
	m_has_post_data = !m_req.getBody().empty();
	std::cout << m_has_post_data << std::endl;
}

void CgiSpawner::spawn(const std::string &script_path)
{
	setupPipes();
	setupEnv();
	m_pid = fork();
	if (m_pid == -1)
	{
		if (m_stdoutPipe[1] != -1)
			close(m_stdoutPipe[1]);
		if (m_stdoutPipe[0] != -1)
			close(m_stdoutPipe[0]);
		if (m_stdinPipe[1] != -1)
			close(m_stdinPipe[1]);
		if (m_stdinPipe[0] != -1)
			close(m_stdinPipe[0]);
	}
	if (m_pid == 0)
		childprocess(script_path);
	parentProcess();
}

void CgiSpawner::setupPipes()
{
	// TODO maybe need to throw ?
	if (pipe(m_stdinPipe) == -1)
	{
		std::cerr << "Pipe creation failed" << std::endl;
		m_stdinPipe[0] = -1;
		m_stdinPipe[1] = -1;
	}
	if (pipe(m_stdoutPipe) == -1)
	{
		std::cerr << "Pipe creation failed" << std::endl;
		m_stdoutPipe[0] = -1;
		m_stdoutPipe[1] = -1;
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
	m_env.push(makeEnvVar("REQUEST_METHOD", m_req.getMethod().getStrType()));
	m_env.push(makeEnvVar("HTTP_HOST", m_req.getHeader(HEADER_HOST)));
	m_env.push(makeEnvVar("SERVER_NAME", SERVER_NAME));
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
	(void) script_path;
	(void) m_config;
	return;
	// const std::string &fileExt = File::determineExtension(, NULL);
	// const std::string &bin	   = m_config.getCgiBin(m_req.getTarget(), );
	// char *const		   argv[3] = {
	// 	   const_cast<char *>(bin.c_str()),
	// 	   const_cast<char *>(script_path.c_str()),
	// 	   NULL};
	// generateEnvp();
	// if (dup2(m_stdinPipe[0], STDIN_FILENO) == -1)
	// 	throw std::runtime_error("Dup2 failed");
	// close(m_stdinPipe[1]);
	// close(m_stdinPipe[0]);
	// if (dup2(m_stdoutPipe[1], STDOUT_FILENO) == -1)
	// 	throw std::runtime_error("Dup2 failed");
	// close(m_stdoutPipe[0]);
	// close(m_stdoutPipe[1]);
	// execve(argv[0], argv, m_envp);
	// perror("execve: ");
	// exit(127);
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
	close(m_stdoutPipe[1]);
	const std::string &body = m_req.getBody();
	write(m_stdinPipe[1], body.data(), body.size());
	close(m_stdinPipe[0]);
	close(m_stdinPipe[1]);
}

bool CgiSpawner::isCgi()
{
	try
	{
		Vec<const Config::CgiParams *>		 cgiParams;
		const Vec<const Config::Location *> &locations = m_req.getActiveLocations();

		for (Vec<const Config::Location *>::const_iterator it = locations.begin(); it != locations.end(); it++)
		{
			if ((*it)->getCgiParams())
				cgiParams.push((*it)->getCgiParams());
		}
		Map<std::string> activeBinMap;
		for (Vec<const Config::CgiParams *>::iterator it = cgiParams.begin(); it != cgiParams.end(); it++)
		{
			const Map<std::string> binMap = (*it)->getBinMap();
			for (Map<std::string>::const_iterator mit = binMap.begin(); mit != binMap.end(); mit++)
			{
				std::string first = mit->first;
				activeBinMap.set(first, mit->second);
			}
		}
		Logger::log("active bin map " + LoggerJson::get(activeBinMap));
		// TODO change to ref
		Vec<std::string> urlSegments = m_req.getSegments();
		for (Vec<std::string>::iterator it = urlSegments.begin(); it != urlSegments.end(); it++)
		{
			std::string fileExtension = File::determineExtension((*it), NULL);
			if (fileExtension.empty())
				continue;
			if (!activeBinMap.has(fileExtension))
				continue;
			m_binPath = activeBinMap.get(fileExtension);
			Logger::log(m_binPath);
			break;
		}
	}
	catch (const FileHandler::FileHandlerException &e)
	{
		std::cerr << e.what() << '\n';
		throw e.getHttpException();
	}
	return false;
}

/**
 *
 * @brief Spawns a CGI script for the client request.
 *
 * This function creates a CgiSpawner instance to handle the CGI script execution.
 * It sets up the necessary pipes and spawns the CGI process.
 * If successful, it creates a CgiPipe to read the CGI output.
 * If spawning fails, it prepares a 500 Internal Server Error response.
 *
 * @param scriptPath The path to the CGI script to be executed.
 *
 */
void CgiSpawner::spawnCgi(const std::string &scriptPath, Client *const client)
{
	spawn(scriptPath);
	if (m_stdoutPipe[0] >= 0)
	{
		CgiPipe *cgiPipeOut = new CgiPipe(m_pid, m_stdoutPipe[0], client);
		EventLoop::addHandler(cgiPipeOut);
	}
	if (m_stdinPipe[1] >= 0)
	{
		CgiPipe *cgiPipeIn = new CgiPipe(m_pid, m_stdinPipe[1], client);
		EventLoop::addHandler(cgiPipeIn);
	}
}
