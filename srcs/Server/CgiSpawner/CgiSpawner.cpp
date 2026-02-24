#include "CgiSpawner.hpp"

#include "webserv.hpp"
#include "FileManager.hpp"
#include "EventLoop.hpp"
#include "CgiPipe.hpp"

#include "Location.hpp"
#include "CgiParams.hpp"
#include "cgiEnv.hpp"

#include "Signals.hpp"
#include <stdio.h>

CgiSpawner::CgiSpawner(const HttpRequest &req, const Config &config) :
	m_req(req),
	m_config(config),
	m_pid(-1),
	m_has_post_data(false)
{
	m_has_post_data = !m_req.getBody().empty();
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
		close(m_stdoutPipe[1]);
		close(m_stdoutPipe[0]);
		close(m_stdinPipe[1]);
		close(m_stdinPipe[0]);
		throw Http500Exception();
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
	if (pipe(m_stdinPipe) == -1)
	{
		m_stdinPipe[0] = -1;
		m_stdinPipe[1] = -1;
		throw Http500Exception();
	}
	if (pipe(m_stdoutPipe) == -1)
	{
		m_stdoutPipe[0] = -1;
		m_stdoutPipe[1] = -1;
		throw Http500Exception();
	}
}

std::string CgiSpawner::makeEnvVar(const std::string &key, const std::string &value)
{
	return key + '=' + value;
}

void CgiSpawner::setupEnv()
{
	m_env.push(makeEnvVar(CGI_AUTH_TYPE, ""));
	m_env.push(makeEnvVar(CGI_GATEWAY_INTERFACE, GATEWAY_INTERFACE));
	m_env.push(makeEnvVar(CGI_PATH_INFO, m_pathInfo));
	m_env.push(makeEnvVar(CGI_PATH_TRANSLATED, m_scriptPath));
	m_env.push(makeEnvVar(CGI_QUERY_STRING, m_req.getQueryString()));
	m_env.push(makeEnvVar(CGI_REMOTE_ADDR, m_req.getHeader(HEADER_REMOTE_ADRESS)));
	m_env.push(makeEnvVar(CGI_REMOTE_HOST, m_req.getHeader(HEADER_REMOTE_HOST)));
	m_env.push(makeEnvVar(CGI_REQUEST_METHOD, m_req.getMethod().getStrType()));
	m_env.push(makeEnvVar(CGI_SCRIPT_NAME, m_scriptName));
	m_env.push(makeEnvVar(CGI_SERVER_NAME, SERVER_NAME));
	m_env.push(makeEnvVar(CGI_SERVER_PORT, Number::toString(m_config.getPort())));
	m_env.push(makeEnvVar(CGI_SERVER_PROTOCOL, PROTOCOL));
	m_env.push(makeEnvVar(CGI_SERVER_SOFTWARE, SERVER_NAME));
	if (m_has_post_data)
	{
		m_env.push(makeEnvVar(CGI_CONTENT_TYPE, m_req.getHeader(HEADER_CONTENT_TYPE)));
		m_env.push(makeEnvVar(CGI_CONTENT_LENGTH, m_req.getHeader(HEADER_CONTENT_LENGTH)));
	}
	m_env.push(makeEnvVar(CGI_HTTP_ACCEPT, m_req.getHeader(HEADER_ACCEPT)));
	m_env.push(makeEnvVar(CGI_HTTP_ACCEPT_LANGUAGE, m_req.getHeader(HEADER_ACCEPT_LANGUAGE)));
	m_env.push(makeEnvVar(CGI_HTTP_ACCEPT_CHARSET, m_req.getHeader(HEADER_ACCEPT_CHARSET)));
	m_env.push(makeEnvVar(CGI_HTTP_ACCEPT_ENCODING, m_req.getHeader(HEADER_ACCEPT_ENCODING)));
	m_env.push(makeEnvVar(CGI_HTTP_TRANSFER_ENCODING, m_req.getHeader(HEADER_TRANSFER_ENCODING)));
	m_env.push(makeEnvVar(CGI_HTTP_CONTENT_ENCODING, m_req.getHeader(HEADER_CONTENT_ENCODING)));
	m_env.push(makeEnvVar(CGI_HTTP_EXPIRES, m_req.getHeader(HEADER_EXPIRES)));
	m_env.push(makeEnvVar(CGI_HTTP_USER_AGENT, m_req.getHeader(HEADER_USER_AGENT)));
	m_env.push(makeEnvVar(CGI_HTTP_COOKIE, m_req.getHeader(HEADER_COOKIE)));
	m_env.push(makeEnvVar(CGI_HTTP_REFERER, m_req.getHeader(HEADER_REFERER)));
}

void CgiSpawner::childprocess()
{
	Signals::setIsChild(true);
	char *const argv[3] = {
		const_cast<char *>(m_binPath.c_str()),
		const_cast<char *>(m_scriptPath.c_str()),
		NULL};
	generateEnvp();
	if (dup2(m_stdinPipe[0], STDIN_FILENO) == -1)
		exitChild();
	close(m_stdinPipe[1]);
	close(m_stdinPipe[0]);
	if (dup2(m_stdoutPipe[1], STDOUT_FILENO) == -1)
		exitChild();
	close(m_stdoutPipe[0]);
	close(m_stdoutPipe[1]);
	execve(argv[0], argv, m_envp.data());
	EventLoop::destroy();
	(*this).~CgiSpawner();
	exit(1);
}

void CgiSpawner::exitChild()
{
	close(m_stdinPipe[0]);
	close(m_stdinPipe[1]);
	close(m_stdoutPipe[0]);
	close(m_stdoutPipe[1]);
	if (!Signals::getIsDestroyed())
	{
		Signals::setIsDestroyed(true);
		EventLoop::destroy();
	}
	(*this).~CgiSpawner();
	exit(1);
}

void CgiSpawner::generateEnvp()
{
	for (size_t i = 0; i < m_env.size(); i++)
		m_envp.push(const_cast<char *>(m_env[i].c_str()));
	m_envp.push(NULL);
}

ssize_t CgiSpawner::getScriptIndex(const Vec<std::string> &urlSegments, const size_t start)
{
	for (size_t i = start; i < urlSegments.size(); i++)
	{
		const std::string fileExtension = FileManager::determineExtension((urlSegments[i]), NULL);
		if (fileExtension.empty() || !m_activeBinMap.has(fileExtension))
			continue;
		m_binPath = m_activeBinMap.get(fileExtension);
		if (!FileManager::hasXPerm(m_binPath))
			throw FileManager::FilePermissionException(m_binPath);
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

void CgiSpawner::fillActiveBinMap()
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
}

bool CgiSpawner::isValidScript()
{
	struct stat st = FileManager::getStat(m_scriptPath);

	if (FileManager::exists(m_scriptPath) && !FileManager::isFolder(st))
	{
		struct stat st = FileManager::getStat(m_scriptPath);
		if (!FileManager::isFile(st))
			throw FileManager::FileDoesNotExistException(m_scriptPath);
		if (!FileManager::hasRPerm(m_scriptPath))
			throw FileManager::FilePermissionException(m_scriptPath);
		return true;
	}
	return false;
}

bool CgiSpawner::isCgi()
{
	try
	{
		fillActiveBinMap();
		const Vec<std::string> &urlSegments = m_req.getSegments();
		const ssize_t			size		= urlSegments.size();
		ssize_t					scriptIndex = getScriptIndex(urlSegments, 0);

		while (scriptIndex != -1 && scriptIndex < size)
		{
			if (isValidScript())
				break;
			scriptIndex = getScriptIndex(urlSegments, scriptIndex + 1);
		}
		if (scriptIndex == -1)
			return false;
		buildPathInfo(urlSegments, scriptIndex);
		buildScriptName(urlSegments, scriptIndex);
	}
	catch (const FileManager::FileException &e)
	{
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
