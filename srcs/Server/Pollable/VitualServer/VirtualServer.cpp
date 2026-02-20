#include "VirtualServer.hpp"
#include "EventLoop.hpp"
#include <fcntl.h>
#include "Number.hpp"
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "Logger.hpp"
#include "httpExceptions.hpp"
#include "Date.hpp"

#include "Client.hpp"
#include "HttpUser.hpp"



VirtualServer::VirtualServer(const Config &config) :
	Pollable(-1),
	m_config(config),
	m_bindIp(config.getIp()),
	m_bindPort(config.getPort())
{
	m_isIPv6 = m_bindIp.find(':') != std::string::npos;
	setupSocket(m_bindIp, m_bindPort);
	m_events = POLLIN;
	EventLoop::addHandler(this);
}

VirtualServer::~VirtualServer()
{
	if (m_fd != -1)
		close(m_fd);
	delete &m_config;
}

/**
 *
 * @brief Gets the bind IP address of the virtual server.
 *
 * @return The bind IP address as a string.
 *
 */
std::string VirtualServer::getBindIp() const
{
	return m_bindIp;
}

/**
 *
 * @brief Gets the bind port of the virtual server.
 *
 * @return The bind port as an unsigned short.
 *
 */
unsigned short VirtualServer::getBindPort() const
{
	return m_bindPort;
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
	return m_config;
}


/**
 *
 * @brief Gets the list of configurations associated with the virtual server.
 *
 * @return A constant reference to a vector of pointers to Config objects.
 *
 */
const Vec<const Config *> &VirtualServer::getConfigs() const
{
	return configs;
}


/**
 *
 * @brief Converts an IPv6 address from string format to a struct in6_addr.
 *
 * @param ip The IPv6 address as a string (e.g., "2001:0db8:85a3:0000:0000:8a2e:0370:7334").
 * @return The IPv6 address as a struct in6_addr.
 * @note This function manually parses the IPv6 address string and constructs the in6_addr structure.
 * It handles both standard and compressed IPv6 formats, including the "::" notation for zero compression.
 *
 * @warning This function assumes the input string is a valid IPv6 address and does not perform error checking.
 *
 */
struct in6_addr VirtualServer::convertIPv6(const std::string &ip)
{
	struct in6_addr addr;
	uint16_t		parts[8]	  = {0};
	uint16_t		value		  = 0;
	int				partIndex	  = 0;
	int				compressIndex = -1;

	std::memset(&addr, 0, sizeof(addr));

	for (size_t i = 0; i <= ip.length(); ++i)
	{
		if (ip[i] == ':')
		{
			if (i > 0 && ip[i - 1] == ':')
			{
				compressIndex = partIndex;
				continue;
			}
			parts[partIndex++] = value;
			value			   = 0;
		}
		else if (ip[i] == '\0')
		{
			parts[partIndex++] = value;
		}
		else
		{
			value = (value << 4) | Number::hexToInt(ip[i]);
		}
	}

	// Handle ::
	if (compressIndex != -1)
	{
		int missing = 8 - partIndex;

		for (int i = partIndex - 1; i >= compressIndex; --i)
			parts[i + missing] = parts[i];

		for (int i = compressIndex; i < compressIndex + missing; ++i)
			parts[i] = 0;
	}

	for (int i = 0; i < 8; ++i)
		addr.s6_addr16[i] = htons(parts[i]);

	return addr;
}



/**
 *
 * @brief Resolves a hostname to an IPv6 address and returns it as a struct in6_addr.
 *
 * @param hostname The hostname to resolve (e.g., "example.com").
 * @return The resolved IPv6 address as a struct in6_addr.
 * @note This function uses the getaddrinfo system call to resolve the hostname to an IPv6 address.
 * It sets up the appropriate hints for IPv6 and SOCK_STREAM, and retrieves the first result.
 *
 * @throws BindException if the hostname cannot be resolved or if getaddrinfo fails.
 *
 */
struct in6_addr VirtualServer::resolveIPv6(const std::string &hostname)
{
	struct addrinfo	 hints;
	struct addrinfo *result;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family	  = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(hostname.c_str(), NULL, &hints, &result) != 0)
		throw BindException();

	struct sockaddr_in6 *addr =
		reinterpret_cast<struct sockaddr_in6 *>(result->ai_addr);

	struct in6_addr ip = addr->sin6_addr;

	freeaddrinfo(result);
	return ip;
}



/**
 *
 * @brief Converts an IPv4 address from string format to a 32-bit integer in network byte order.
 *
 * @param ip The IPv4 address as a string (e.g., "192.168.1.1").
 * @return The IPv4 address as a 32-bit integer in network byte order.
 * @note This function manually parses the IPv4 address string and constructs the integer representation.
 * It processes each octet of the IP address, shifting and combining them into a single 32-bit value.
 *
 * @warning This function assumes the input string is a valid IPv4 address and does not perform error checking.
 *
 */
uint32_t VirtualServer::convertIPv4(const std::string &ip)
{
	uint32_t addr  = 0;
	uint32_t value = 0;
	int		 shift = 24;

	for (size_t i = 0; i <= ip.length(); ++i)
	{
		if (ip[i] == '.' || ip[i] == '\0')
		{
			addr |= (value << shift);
			value = 0;
			shift -= 8;
		}
		else
			value = value * 10 + (ip[i] - '0');
	}
	return htonl(addr);
}


/**
 *
 * @brief Resolves a hostname to an IPv4 address and returns it as a 32-bit integer in network byte order.
 *
 * @param hostname The hostname to resolve (e.g., "example.com").
 * @return The resolved IPv4 address as a 32-bit integer in network byte order.
 * @note This function uses the getaddrinfo system call to resolve the hostname to an IPv4 address.
 * It sets up the appropriate hints for IPv4 and SOCK_STREAM, and retrieves the first result.
 *
 * @throws BindException if the hostname cannot be resolved or if getaddrinfo fails.
 *
 */
uint32_t VirtualServer::resolveIPv4(const std::string &hostname)
{
	struct addrinfo	 hints;
	struct addrinfo *result;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family	  = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(hostname.c_str(), NULL, &hints, &result) != 0)
		throw BindException();

	struct sockaddr_in *addr = reinterpret_cast<struct sockaddr_in *>(result->ai_addr);
	const uint32_t		ip	 = addr->sin_addr.s_addr;
	freeaddrinfo(result);

	return ip;
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
 * @param ip The IP address to bind to (e.g., "192.168.1.1").
 * @param port The port number to bind to.
 *
 * @throws BindException if the bind operation fails.
 * @throws ListenException if the listen operation fails.
 *
 */
void VirtualServer::bindAndListen(
	int					 sockFd,
	const std::string	&ip,
	const unsigned short port)
{
	struct sockaddr_storage storage;
	std::memset(&storage, 0, sizeof(storage));
	socklen_t addrlen;

	if (ip.find(':') != std::string::npos)
	{
		struct sockaddr_in6 *addr6 = reinterpret_cast<struct sockaddr_in6 *>(&storage);
		addr6->sin6_family		   = AF_INET6;
		addr6->sin6_port		   = htons(port);
		addr6->sin6_addr		   = isalpha(ip[0]) ? resolveIPv6(ip) : convertIPv6(ip);
		addrlen					   = sizeof(struct sockaddr_in6);
	}
	else
	{
		struct sockaddr_in *addr4 = reinterpret_cast<struct sockaddr_in *>(&storage);
		addr4->sin_family		  = AF_INET;
		addr4->sin_port			  = htons(port);
		addr4->sin_addr.s_addr	  = isalpha(ip[0]) ? resolveIPv4(ip) : convertIPv4(ip);
		addrlen					  = sizeof(struct sockaddr_in);
	}

	if (bind(sockFd, reinterpret_cast<struct sockaddr *>(&storage), addrlen) < 0)
		throw BindException();
	if (listen(sockFd, SOMAXCONN) < 0)
		throw ListenException();
}


/**
 *
 * @brief Sets up the server socket by creating it, setting options, binding, and listening.
 *
 * @param ip The IP address to bind to (IPV4 or IPv6).
 * @param port The port number to bind to.
 *
 * @return true if the socket setup was successful, false otherwise.
 *
 */
bool VirtualServer::setupSocket(
	const std::string &ip,
	unsigned short	   port)
{
	try
	{
		m_fd = socket(m_isIPv6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
		if (m_fd < 0)
			throw SocketCreationException();
		setReusable(m_fd);
		bindAndListen(m_fd, ip, port);
		Logger::info("VirtualServer bound to " + Logger::get(ip) + ":" + Logger::get(port))
			.info("Go to http://" + Logger::get(ip) + ":" + Logger::get(port) + " to access the server");
		return true;
	}
	catch (const ServerException &e)
	{
		if (m_fd >= 0)
		{
			close(m_fd);
			m_fd = -1;
		}
		std::cerr << e.what() << " (" << ip << ":" << port << ")" << std::endl;
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
		std::cerr << "Failed to set client socket to non-blocking mode: " << strerror(errno) << std::endl;
		close(clientFd);
		return;
	}
	new Client(clientFd, m_config, *this);
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
		std::cerr << "Accept failed on fd " << m_fd << ": " << strerror(errno) << std::endl;
		return;
	}
	createClient(clientFd);
}

void VirtualServer::onPollOut()
{
}

void VirtualServer::onPollErr()
{
	std::cerr << "VirtualServer error on fd " << m_fd << std::endl;
}

void VirtualServer::onPollHangUp()
{
}

void VirtualServer::onTimeOut()
{
}

const std::string VirtualServer::createCookie(const std::string name, const std::string pswd)
{
	// TODO Check if domain is the right string
	for (UserCIt it = m_users.begin(); it != m_users.end(); it++)
	{
		if (name == it->getName())
			throw Http409Exception();
	}
	HttpUser newUser(name, pswd, m_config.getServerName(), m_config.getPort());
	newUser.stringifyCookie();
	m_users.push(newUser);
	return newUser.getStringifiedCookie();
}

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

const std::string &VirtualServer::findCookie(const std::string &name, const std::string &pswd)
{
	for (UserIt it = m_users.begin(); it != m_users.end(); it++)
	{
		if (it->getName() == name && it->getPswd() == Number::toString(AuthToken::computeHash(pswd + it->getSalt())))
		{
			if (!it->hasValidToken())
			{
				it->refreshToken();
				Logger::error("Expired Cookie, created a new one");
			}
			return it->getStringifiedCookie();
		}
	}
	throw Http401Exception();
}
