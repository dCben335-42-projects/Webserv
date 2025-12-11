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
#include <sys/types.h> // sometimes needed for send()
#include <sys/socket.h>

HttpServer::HttpServer(unsigned short port) : m_socket(-1), m_port(port)
{
	m_socket = socket(SOCKET_IPV4, SOCKET_TCP, SOCKET_PROTOCOL);
	if (m_socket < 0)
		throw std::runtime_error("Failed to create socket");
	int opt = 1;
	if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("setsockopt failed");

	std::memset(&m_address, 0, sizeof(m_address));
	m_address.sin_family	  = SOCKET_IPV4;
	m_address.sin_addr.s_addr = SOCKET_INTERFACE;
	m_address.sin_port		  = htons(m_port);

	if (bind(m_socket, (struct sockaddr *) &m_address, sizeof(m_address)) < 0)
		throw std::runtime_error("Bind failed");
	if (listen(m_socket, SOMAXCONN) < 0)
		throw std::runtime_error("Listen failed");

	std::cout << "Server started on port " << "http://localhost:" << m_port << std::endl;
	createPoll(m_socket);
}

HttpServer::~HttpServer()
{
	if (m_socket > 0)
		close(m_socket);
}


void HttpServer::createPoll(int socket)
{
	pollfd pollFd;

	pollFd.fd	  = socket;
	pollFd.events = POLLIN;
	m_pollFds.push_back(pollFd);
}

void HttpServer::createPollClient(void)
{
	struct sockaddr_in clientAddr;
	socklen_t		   clientLen = sizeof(clientAddr);
	int				   clientSocket;

	try
	{
		clientSocket = accept(m_socket, (struct sockaddr *) &clientAddr, &clientLen);
		if (clientSocket < 0)
			throw std::runtime_error("Accept failed");
		int flags = fcntl(clientSocket, F_GETFL, 0);
		if (flags < 0)
			throw std::runtime_error("fcntl F_GETFL failed");
		if (fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK) < 0)
			throw std::runtime_error("fcntl F_SETFL failed");
		m_clients.insert(std::make_pair(clientSocket, HttpClient(clientSocket)));
		createPoll(clientSocket);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		if (clientSocket > 0)
			close(clientSocket);
	}
}

void HttpServer::run()
{
	while (true)
	{
		for (size_t i = 1; i < m_pollFds.size(); ++i)
		{
			int									fd = m_pollFds[i].fd;
			std::map<int, HttpClient>::iterator it = m_clients.find(fd);
			if (it != m_clients.end())
			{
				HttpClient &client	= it->second;
				m_pollFds[i].events = 0; // meanwhile reset events
				if (client.getState() == READING)
					m_pollFds[i].events |= POLLIN;
				if (client.getState() == WRITING)
					m_pollFds[i].events |= POLLOUT;
			}
		}

		int ready = poll(&m_pollFds[0], m_pollFds.size(), -1);
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

			if (m_pollFds[i].fd == m_socket)
				createPollClient();
			else
			{
				int									fd = m_pollFds[i].fd;
				std::map<int, HttpClient>::iterator it = m_clients.find(fd);
				if (it != m_clients.end())
				{
					HttpClient &client = it->second;

					if (m_pollFds[i].revents & POLLIN)
						client.onRead();
					if (m_pollFds[i].revents & POLLOUT)
						client.onWrite(m_pollFds[i]);
					if (client.getState() == CLOSED)
					{
						close(fd);
						m_clients.erase(it);
						m_pollFds.erase(m_pollFds.begin() + i);
						i--;
					}
				}
				else
				{
					close(fd);
					m_pollFds.erase(m_pollFds.begin() + i);
					i--;
				}
			}
		}
	}
}
