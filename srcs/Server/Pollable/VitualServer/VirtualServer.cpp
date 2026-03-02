#include "VirtualServer.hpp"
#include "EventLoop.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>

#include "Logger.hpp"
#include "httpExceptions.hpp"

#include "Client.hpp"
#include "HttpUser.hpp"

#include "HttpAuth.hpp"
#include "log.hpp"


VirtualServer::VirtualServer(
	const SocketAddress &address,
	const Config		&http,
	const ServerConfigs &serverConfigs) :
	Pollable(-1),
	m_defaultConf(http),
	m_configs(serverConfigs),
	m_bindAddress(address)
{
	if (!setupSocket())
		onPollErr();
	else
		m_events = POLLIN;
}

VirtualServer::~VirtualServer()
{
	if (m_fd != -1)
		close(m_fd);
}


/**
 *
 * @brief Gets the configuration of the virtual server.
 *
 * @return A constant reference to the Config object.
 *
 */
const Config &VirtualServer::getConfig() const
{
	return m_defaultConf;
}


/**
 *
 * @brief Gets the list of configurations associated with the virtual server.
 *
 * @return A constant reference to a vector of pointers to Config objects.
 *
 */
const ServerConfigs &VirtualServer::getConfigs() const
{
	return m_configs;
}





/**
 *
 * @brief Sets the SO_REUSEADDR option on the given socket file descriptor.
 *
 * @param sockFd The socket file descriptor on which to set the option.
 *
 * @throws SocketOptionException if setsockopt fails.
 *
 */
void VirtualServer::setReusable(int sockFd)
{
	int opt = 1;

	if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw SocketOptionException();
}


/**
 *
 * @brief Binds the socket to the specified IP address and port, and starts listening for incoming connections.
 *
 * @param sockFd The socket file descriptor to bind and listen on.
 *
 * @throws BindException if the bind operation fails.
 * @throws ListenException if the listen operation fails.
 *
 */
void VirtualServer::bindAndListen(int sockFd)
{
	if (bind(sockFd, m_bindAddress.getAddr(), m_bindAddress.getLength()) < 0)
		throw BindException();
	if (listen(sockFd, SOMAXCONN) < 0)
		throw ListenException();
}


/**
 *
 * @brief Sets up the server socket by creating it, setting options, binding, and listening.
 *
 * @return true if the socket setup was successful, false otherwise.
 *
 */
bool VirtualServer::setupSocket()
{
	try
	{
		m_fd = socket(m_bindAddress.isIPv6() ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
		if (m_fd < 0)
			throw SocketCreationException();
		if (fcntl(m_fd, F_SETFL, O_NONBLOCK) < 0)
			throw SocketCreationException();
		setReusable(m_fd);
		bindAndListen(m_fd);
		// TODO
		Logger::info("Go to http://" + Logger::get(m_bindAddress) + " to access the server");
		return true;
	}
	catch (const ServerException &e)
	{
		if (m_fd >= 0)
		{
			close(m_fd);
			m_fd = -1;
		}
		std::cerr << e.what() << " (" << Logger::get(m_bindAddress) << ")" << std::endl;
		// TODO
		//  std::cerr << e.what() << " (" << ip << ":" << port << ")" << std::endl;
		return false;
	}
}



/**
 *
 * @brief Creates a new Client and adds it to the event loop.
 *
 * @param clientFd The file descriptor of the accepted client connection.
 *
 * @note It set the client socket to non-blocking mode to ensure that
 * the server can handle multiple clients concurrently without
 * blocking on any single client.
 *
 * @note If setting the socket to non-blocking mode fails,
 * it logs the error and closes the client socket.
 *
 */
void VirtualServer::createClient(int clientFd)
{
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
	{
		// TODO Logger::openFile(errorFile).custom(ERROR_MSG_FCNTL_FAILED);
		close(clientFd);
		return;
	}
	new Client(clientFd, m_defaultConf, *this);
}

bool VirtualServer::isTimedOut(const time_t currentTime) const
{
	(void) currentTime;
	return false;
}

/**
 *
 * @brief Handles incoming connections on the virtual server.
 *
 * @note This method is called when a new connection is ready to be accepted.
 *
 */
void VirtualServer::onPollIn()
{
	struct sockaddr_in clientAddr;
	socklen_t		   clientLen = sizeof(clientAddr);

	int clientFd = accept(m_fd, (struct sockaddr *) &clientAddr, &clientLen);
	if (clientFd < 0)
	{
		// TODO Logger::openFile(errorFile).custom(ERROR_MSG_ACCEPT_FAILED);
		return;
	}
	createClient(clientFd);
}

void VirtualServer::onPollOut()
{
}

void VirtualServer::onPollErr()
{
	m_connectionClosed = true;
	m_events		   = 0;
	std::cerr << "VirtualServer error on fd " << m_fd << std::endl;
	// TODO Logger::openFile(errorFile).custom(ERROR_MSG_UNKNOWN);
}

void VirtualServer::onPollHangUp()
{
}

void VirtualServer::onTimeOut()
{
}


/**
 *
 * @brief Creates a user for the auth system, and a cookie associated with this user
 *
 * @param name The name of the user
 * @param pswd The password of the user
 *
 * @throw Http409Exception() if the user already exists
 *
 */
const std::string VirtualServer::createUser(const std::string name, const std::string pswd)
{
	for (UserCIt it = m_users.begin(); it != m_users.end(); it++)
	{
		if (name == it->getName())
			throw Http409Exception();
	}
	HttpUser newUser(name, pswd, m_defaultConf.getServerNames()[0], m_defaultConf.getAddress().getPort());
	newUser.stringifyCookie();
	m_users.push(newUser);
	return newUser.getStringifiedCookie();
}


/**
 *
 * @brief Checks if the Server has a specific user based on a hash
 *
 * @param hash The hash used to find the user in the server
 *
 */
bool VirtualServer::hasUser(const std::string &hash) const
{
	AuthToken token(hash);
	for (UserCIt it = m_users.begin(); it != m_users.end(); it++)
	{
		if (token == it->getToken() && it->hasValidToken())
			return true;
	}
	return false;
}


/**
 *
 * @brief Returns the cookie corresonding with the user settings in params
 * Hashes thoses params and checks if the hash is already stored
 *
 * @param name The name of the user
 * @param pswd The password of the user
 * @param authLoc The location of authentification, used to redirect the user when invalid credentials are entered
 *
 * @throw Http301Exception() in case the User is not found
 *
 */
const std::string &VirtualServer::findUser(const std::string &name, const std::string &pswd, const std::string &authLoc)
{
	for (UserIt it = m_users.begin(); it != m_users.end(); it++)
	{
		if (it->getName() == name && it->getPswd() == Number::toString(AuthToken::computeHash(pswd + it->getSalt())))
		{
			if (!it->hasValidToken())
				it->refreshToken();
			return it->getStringifiedCookie();
		}
	}
	throw Http301Exception(authLoc + "?" + AUTH_LOGIN_PSWD_KEY + "=" + AUTH_INVALID_CREDENTIALS);
}
