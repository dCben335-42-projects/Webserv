#include "VirtualServer.hpp"
#include "EventLoop.hpp"
#include <unistd.h>
#include <errno.h>
#include "webserv.hpp"
#include <cstring>
#include <fcntl.h>
#include <arpa/inet.h>

VirtualServer::VirtualServer(const Config &config) :
	Pollable(-1),
	m_config(config),
	m_bindIp(config.getIp()),
	m_bindPort(config.getPort())
{
	if (!setupSocket(m_bindIp, m_bindPort))
		throw std::runtime_error("VirtualServer socket setup failed: " + m_bindIp);
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
 * @brief Sets up the server socket for the virtual server.
 *
 * @param ip The IP address to bind to.
 * @param port The port number to bind to.
 *
 * @return true if the socket setup was successful, false otherwise.
 *
 */
bool VirtualServer::setupSocket(const std::string &ip, unsigned short port)
{
	struct sockaddr_in serverAddr;
	int				   sockFd;
	int				   opt = 1;

	sockFd = socket(SOCKET_IPV4, SOCKET_TCP, SOCKET_PROTOCOL);
	if (sockFd < 0)
	{
		std::cerr << "Socket creation failed for " << ip << ":" << port << std::endl;
		return false;
	}

	if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "setsockopt failed" << std::endl;
		close(sockFd);
		return false;
	}
	serverAddr.sin_family	   = SOCKET_IPV4;
	serverAddr.sin_addr.s_addr = SOCKET_INTERFACE;
	serverAddr.sin_port		   = htons(port);

	if (bind(sockFd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
	{
		std::cerr << "Bind failed on " << ip << ":" << port << std::endl;
		close(sockFd);
		return false;
	}
	if (listen(sockFd, SOMAXCONN) < 0)
	{
		std::cerr << "Listen failed on " << ip << ":" << port << std::endl;
		close(sockFd);
		return false;
	}
	m_fd = sockFd;
	std::cout << "[VirtualServer] Bound to " << ip << ":" << port << "\"," << "try to go on http://" << m_config.getDomain() << ":" << port << "/" << std::endl;
	return true;
}



Client *VirtualServer::createClient(int clientFd)
{
	// if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
	// {
	// 	std::cerr << "Failed to set client socket to non-blocking mode: " << strerror(errno) << std::endl;
	// 	close(clientFd);
	// 	return NULL;
	// }
	Client *client = new Client(clientFd, m_config);
	return client;
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