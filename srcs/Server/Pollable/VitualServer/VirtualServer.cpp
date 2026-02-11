#include "VirtualServer.hpp"
#include "EventLoop.hpp"
#include <fcntl.h>
#include "Number.hpp"
#include "Client.hpp"
#include <unistd.h>
#include "Logger.hpp"


VirtualServer::VirtualServer(const Config &config) :
	Pollable(-1),
	m_config(config),
	m_bindIp(config.getIp()),
	m_bindPort(config.getPort())
{
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
		addr6->sin6_addr		   = convertIPv6(ip);
		addrlen					   = sizeof(struct sockaddr_in6);
	}
	else
	{
		struct sockaddr_in *addr4 = reinterpret_cast<struct sockaddr_in *>(&storage);
		addr4->sin_family		  = AF_INET;
		addr4->sin_port			  = htons(port);
		addr4->sin_addr.s_addr	  = convertIPv4(ip);
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
		m_fd = socket(AF_INET, SOCK_STREAM, 0);
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
	}
	new Client(clientFd, m_config);
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