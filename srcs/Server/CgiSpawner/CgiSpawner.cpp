#include "CgiSpawner.hpp"
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdlib>
#include "config.hpp"
#include <unistd.h>
#include "File.hpp"
#include "Config.hpp"
#include "Location.hpp"
#include "CgiParams.hpp"
#include "Logger.hpp"
#include "FileHandler.hpp"
#include "CgiPipe.hpp"
#include "EventLoop.hpp"
#include "Signals.hpp"

CgiSpawner::CgiSpawner(const HttpRequest &req, const Config &config) :
	m_req(req),
	m_config(config),
	m_pid(-1),
	m_has_post_data(false)
{
	m_has_post_data = !m_req.getBody().empty();
	std::cout << m_has_post_data << std::endl;
}

CgiSpawner::~CgiSpawner()
{
}

void CgiSpawner::spawn()
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
		childprocess();
	close(m_stdoutPipe[1]);
	close(m_stdinPipe[0]);
	if (!m_has_post_data)
	{
		close(m_stdinPipe[1]);
		m_stdinPipe[1] = -1;
	}
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
	m_env.push(makeEnvVar("AUTH_TYPE", ""));
	m_env.push(makeEnvVar("GATEWAY_INTERFACE", GATEWAY_INTERFACE));
	m_env.push(makeEnvVar("PATH_INFO", m_pathInfo));
	m_env.push(makeEnvVar("PATH_TRANSLATED", m_scriptPath));
	m_env.push(makeEnvVar("QUERY_STRING", m_req.getQueryString()));
	m_env.push(makeEnvVar("REMOTE_ADDR", m_req.getHeader("Remote Adress")));
	m_env.push(makeEnvVar("REMOTE_HOST", m_req.getHeader("Remote Host")));
	m_env.push(makeEnvVar("REQUEST_METHOD", m_req.getMethod()));
	m_env.push(makeEnvVar("SCRIPT_NAME", m_scriptName));
	m_env.push(makeEnvVar("SERVER_NAME", PROJECT_NAME));
	m_env.push(makeEnvVar("SERVER_PORT", Number::toString(m_config.getPort())));
	m_env.push(makeEnvVar("SERVER_PROTOCOL", PROTOCOL));
	m_env.push(makeEnvVar("SERVER_SOFTWARE", SERVER_NAME));
	if (m_has_post_data)
	{
		m_env.push(makeEnvVar("CONTENT_TYPE", m_req.getHeader(HEADER_CONTENT_TYPE)));
		m_env.push(makeEnvVar("CONTENT_LENGTH", m_req.getHeader(HEADER_CONTENT_LENGTH)));
	}
}

#include <stdio.h>
#include <ctime>
void CgiSpawner::childprocess()
{
	Signals::setIsChild(true);
	char *const argv[3] = {
		const_cast<char *>(m_binPath.c_str()),
		const_cast<char *>(m_scriptPath.c_str()),
		NULL};
	generateEnvp();
	if (dup2(m_stdinPipe[0], STDIN_FILENO) == -1)
		throw std::runtime_error("Dup2 failed");
	close(m_stdinPipe[1]);
	close(m_stdinPipe[0]);
	if (dup2(m_stdoutPipe[1], STDOUT_FILENO) == -1)
		throw std::runtime_error("Dup2 failed");
	close(m_stdoutPipe[0]);
	close(m_stdoutPipe[1]);
	execve(argv[0], argv, m_envp);
	perror("execve");
	EventLoop::destroy();
	(*this).~CgiSpawner();
	exit(127);
}

void CgiSpawner::generateEnvp()
{
	size_t i;
	for (i = 0; i < m_env.size(); i++)
		m_envp[i] = const_cast<char *>(m_env[i].c_str());
	m_envp[i] = NULL;
}

ssize_t CgiSpawner::getScriptIndex(const Vec<std::string> &urlSegments)
{
	for (size_t i = 0; i < urlSegments.size(); i++)
	{
		std::string fileExtension = File::determineExtension((urlSegments[i]), NULL);
		if (fileExtension.empty())
			continue;
		if (!m_activeBinMap.has(fileExtension))
			continue;
		m_binPath = m_activeBinMap.get(fileExtension);
		Logger::log(m_binPath);
		m_scriptPath = m_req.buildResourcePath(i);
		return i;
	}
	return -1;
}

void CgiSpawner::buildPathInfo(const Vec<std::string> &urlSegments, ssize_t scriptIndex)
{
	for (Vec<std::string>::const_iterator it = urlSegments.begin() + scriptIndex + 1; it != urlSegments.end(); it++)
	{
		m_pathInfo.append("/");
		m_pathInfo.append(*it);
	}
	const std::string target = m_req.getTarget();
	if (target.at(target.size() - 1) == '/')
		m_pathInfo.append("/");
}

void CgiSpawner::buildScriptName(const Vec<std::string> &urlSegments, ssize_t scriptIndex)
{
	for (Vec<std::string>::const_iterator it = urlSegments.begin(); it != urlSegments.begin() + scriptIndex + 1; it++)
	{
		m_scriptName.append("/");
		m_scriptName.append(*it);
	}
}

// TODO Need access() the binary but where ?
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
		for (Vec<const Config::CgiParams *>::iterator it = cgiParams.begin(); it != cgiParams.end(); it++)
		{
			const Map<std::string> binMap = (*it)->getBinMap();
			for (Map<std::string>::const_iterator mit = binMap.begin(); mit != binMap.end(); mit++)
			{
				std::string first = mit->first;
				m_activeBinMap.set(first, mit->second);
			}
		}
		Logger::log("Active bin map " + LoggerJson::get(m_activeBinMap));
		const Vec<std::string> &urlSegments = m_req.getSegments();
		ssize_t					scriptIndex = getScriptIndex(urlSegments);
		if (scriptIndex == -1)
			return false;
		if (!FileHandler::hasReadPermission(m_scriptPath))
			throw Http403Exception();
		buildPathInfo(urlSegments, scriptIndex);
		buildScriptName(urlSegments, scriptIndex);
	}
	catch (const FileHandler::FileHandlerException &e)
	{
		std::cerr << e.what() << '\n';
		throw e.getHttpException();
	}
	return true;
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
void CgiSpawner::spawnCgi(Client *const client)
{
	spawn();
	if (m_stdoutPipe[0] >= 0)
	{
		CgiPipe *cgiPipeOut = new CgiPipe(m_pid, m_stdoutPipe[0], client);
		EventLoop::addHandler(cgiPipeOut);
		cgiPipeOut->setEvents(POLLIN);
	}
	if (m_stdinPipe[1] >= 0)
	{
		CgiPipe *cgiPipeIn = new CgiPipe(-1, m_stdinPipe[1], client);
		EventLoop::addHandler(cgiPipeIn);
		cgiPipeIn->setEvents(POLLOUT);
	}
}
