#include "HttpServer.hpp"
#include "config.hpp"
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include "FileHandler.hpp"
#include "httpErrors.hpp"
#include <fcntl.h>
#include <iostream>
#include <cerrno>
#include <sys/types.h>
#include "Map.hpp"
#include <sys/socket.h>

HttpServer::HttpServer()
{
	createPollServer(DEFAULT_PORT);
	createPollServer(9090);
}

HttpServer::~HttpServer()
{
	for (size_t i = 0; i < m_pollFds.size(); ++i)
		close(m_pollFds[i].fd);
}

void HttpServer::createPollServer(const unsigned short port)
{
	struct sockaddr_in serverAddr;
	int				   sock_fd = socket(SOCKET_IPV4, SOCKET_TCP, SOCKET_PROTOCOL);
	int				   opt	   = 1;

	if (sock_fd < 0)
		throw std::runtime_error("Failed to create socket");
	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("setsockopt failed");

	std::memset(&serverAddr, 0, sizeof(serverAddr));
	// serverAddr.sin_addr.s_addr = SOCKET_INTERFACE;
	serverAddr.sin_family	   = SOCKET_IPV4;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port		   = htons(port);

	if (bind(sock_fd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
		throw std::runtime_error("Bind failed");
	if (listen(sock_fd, SOMAXCONN) < 0)
		throw std::runtime_error("Listen failed");

	std::cout << "Server started on port " << "http://localhost:" << port << std::endl;
	createPoll(sock_fd);
	m_hosts.push(sock_fd);
}

// ANCHOR Private poll methods
void HttpServer::createPoll(const int socket)
{
	pollfd pollFd;

	pollFd.fd	  = socket;
	pollFd.events = POLLIN;
	m_pollFds.push(pollFd);
}

void HttpServer::deleteClient(const int &clientFd, const size_t &index)
{
	m_clients.erase(clientFd);
	m_pollFds.erase(m_pollFds.begin() + index);
	close(clientFd);
}


void HttpServer::createPollClient(const int serverFd)
{
	struct sockaddr_in clientAddr;
	socklen_t		   clientLen	= sizeof(clientAddr);
	int				   clientSocket = -1;

	try
	{
		clientSocket = accept(serverFd, (struct sockaddr *) &clientAddr, &clientLen);
		if (clientSocket < 0)
			throw std::runtime_error("Accept failed");
		int flags = fcntl(clientSocket, F_GETFL, 0);
		if (flags < 0)
			throw std::runtime_error("fcntl F_GETFL failed");
		if (fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK) < 0)
			throw std::runtime_error("fcntl F_SETFL failed");
		m_clients.set(clientSocket, HttpClient(clientSocket));
		createPoll(clientSocket);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		if (clientSocket > 0)
			close(clientSocket);
	}
}

void HttpServer::updatePollStatus()
{
	for (size_t i = 0; i < m_pollFds.size(); ++i)
	{
		HttpClient *client = NULL;
		if (m_clients.try_get(m_pollFds[i].fd, client))
		{
			m_pollFds[i].events = 0;
			if (client->getState() == READING)
				m_pollFds[i].events |= POLLIN;
			if (client->getState() == WRITING)
				m_pollFds[i].events |= POLLOUT;
		}
	}
}

void HttpServer::run()
{
	int			ready;
	HttpClient *client		 = NULL;
	const int	POLL_TIMEOUT = 1000;

	while (true)
	{
		updatePollStatus();
		ready = poll(&m_pollFds[0], m_pollFds.size(), POLL_TIMEOUT);
		if (ready < 0)
		{
			std::cerr << "Poll error\n";
			continue;
		}
		for (size_t i = 0; i < m_pollFds.size() && ready > 0; ++i)
		{
			if (m_pollFds[i].revents == 0)
				continue;
			--ready;

			const int fd = m_pollFds[i].fd;
			if (m_pollFds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
			{
				deleteClient(fd, i--);
				continue;
			}
			if (m_hosts.contains(fd))
			{
				createPollClient(fd);
				continue;
			}
			if (m_clients.try_get(fd, client))
			{
				if (m_pollFds[i].revents & POLLIN)
					client->onRead();
				if (m_pollFds[i].revents & POLLOUT)
					client->onWrite(m_pollFds[i]);
				if (client->getState() != CLOSED)
					continue;
			}
			deleteClient(fd, i--);
		}
	}
}
