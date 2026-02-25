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
#include <fcntl.h>

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


/**
 *
 * @brief Create pipes to establish communication between the child and parent processes
 * Also sets those pipes to non-blocking state with fcntl
 *
 * @throw Http500Exception in case pipe() or fcntl() fails
 *
 */
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
	if (fcntl(m_stdinPipe[1], F_SETFL, O_NONBLOCK) == -1)
	{
		closePipes();
		throw Http500Exception();
	}
	if (fcntl(m_stdoutPipe[0], F_SETFL, O_NONBLOCK) == -1)
	{
		closePipes();
		throw Http500Exception();
	}
}


/**
 *
 * @brief Util secured function to close the pipes
 *
 */
void CgiSpawner::closePipes()
{
	if (m_stdinPipe[0] != -1)
		close(m_stdinPipe[0]);
	if (m_stdinPipe[1] != -1)
		close(m_stdinPipe[1]);
	if (m_stdoutPipe[0] != -1)
		close(m_stdoutPipe[0]);
	if (m_stdoutPipe[1] != -1)
		close(m_stdoutPipe[1]);
}

/**
 *
 * @brief Set up child and parent processes communication
 *
 * @throw Http500Exception in case fork() fails
 *
 */
void CgiSpawner::spawn()
{
	setupPipes();
	setupEnv();
	m_pid = fork();
	if (m_pid == -1)
	{
		closePipes();
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

std::string CgiSpawner::makeEnvVar(const std::string &key, const std::string &value)
{
	return key + '=' + value;
}


/**
 *
 * @brief Fill the env vector that will be sent to the child process
 *
 */
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

/**
 *
 * @brief Child process handler, it generates the envp char** for execve,
 * dup the pipes to standard I/O and call execve with the binary and script to launch
 *
 * @note If somewhat execve fails, the child free all the memory allocated and destroy itself before exiting
 *
 */
void CgiSpawner::childprocess()
{
	Signals::setIsChild(true);
	char *const argv[3] = {
		const_cast<char *>(m_binPath.c_str()),
		const_cast<char *>(m_scriptPath.c_str()),
		NULL};
	generateEnvp();
	if (dup2(m_stdinPipe[0], STDIN_FILENO) == -1)
		exitChild(true);
	if (dup2(m_stdoutPipe[1], STDOUT_FILENO) == -1)
		exitChild(true);
	closePipes();
	execve(argv[0], argv, m_envp.data());
	exitChild(false);
}


/**
 *
 * @brief Util function to exit the child process in case of dup2() and execve fails()
 *
 */
void CgiSpawner::exitChild(bool needFdClose)
{
	if (needFdClose)
		closePipes();
	if (!Signals::getIsDestroyed())
	{
		Signals::setIsDestroyed(true);
		EventLoop::destroy();
	}
	(*this).~CgiSpawner();
	exit(1);
}


/**
 *
 * @brief Util function to convert the Vec<std::string> to an Vec<char *> to comply with execve
 *
 */
void CgiSpawner::generateEnvp()
{
	for (size_t i = 0; i < m_env.size(); i++)
		m_envp.push(const_cast<char *>(m_env[i].c_str()));
	m_envp.push(NULL);
}


/**
 *
 * @brief Parses the urlSegments vector from a specific point and finds the script to launch,
 * checks if it's extension is valid (e.g present in the binMap from the config), and if the user
 * has the rights to launch the binary associated with the extension
 *
 * @note The path of the binary and the path of the script or assigned here
 *
 * @param urlSegments A vector filled with each url segment
 * @param start The index of the start of the research in the vector
 *
 * @throw FilePermissionException if the binary is not executable
 *
 */
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

/**
 *
 * @brief Builds the PATH_INFO variable for the CGI environment
 *
 * @param urlSegments A vector filled with each url segment
 * @param scriptIndex The index of the script, since the PATH_INFO variable contains
 * all the content between the script segment and the end of the vector
 *
 */
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

/**
 *
 * @brief Builds the SCRIPT_NAME variable for the CGI environment
 *
 * @param urlSegments A vector filled with each url segment
 * @param scriptIndex The index of the script, to create the full name of the script
 * based on his segment
 *
 */
void CgiSpawner::buildScriptName(const Vec<std::string> &urlSegments, ssize_t scriptIndex)
{
	for (Vec<std::string>::const_iterator it = urlSegments.begin(); it != urlSegments.begin() + scriptIndex + 1; it++)
	{
		m_scriptName.append("/");
		m_scriptName.append(*it);
	}
}


/**
 *
 * @brief Fill the map of active CGI binaries, based on the active locations Vector
 *
 */
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

/**
 *
 * @brief Check if the script is a valid file that can be read and used for the CGI
 *
 */
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


/**
 *
 * @brief Checks if the requested target is a CGI script
 *
 */
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
