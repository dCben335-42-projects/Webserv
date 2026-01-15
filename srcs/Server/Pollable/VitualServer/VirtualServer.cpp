// VirtualServer.cpp
#include "VirtualServer.hpp"
#include "EventLoop.hpp"
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <arpa/inet.h>
#include <errno.h>

VirtualServer::VirtualServer(const std::string &ip, unsigned short port, const Config &config) : Pollable(-1), m_bindIp(ip), m_bindPort(port), m_config(config)
{
	if (!setupSocket(ip, port))
	{
		throw std::runtime_error("VirtualServer socket setup failed: " + ip);
	}
	m_events = POLLIN;
	EventLoop::addHandler(this);
}

VirtualServer::~VirtualServer()
{
	if (m_fd != -1)
		close(m_fd);
}

std::string VirtualServer::getBindIp() const
{
	return m_bindIp;
}

unsigned short VirtualServer::getBindPort() const
{
	return m_bindPort;
}

std::string VirtualServer::getDocumentRoot() const
{
	return m_config.root;
}

std::string VirtualServer::getCgiBin() const
{
	return m_config.cgiBin;
}

#include "config.hpp"

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
	std::memset(&serverAddr, 0, sizeof(serverAddr));
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
	std::cout << "[VirtualServer] Bound to " << ip << ":" << port
			  << " root=\"" << m_config.root << "\"," << "try to go on http://" << m_config.domain << ":" << port << "/" << std::endl;
	return true;
}

// Client *VirtualServer::createClient(int clientFd)
// {
// 	fcntl(clientFd, F_SETFL, O_NONBLOCK);
// 	Client *client = new Client(clientFd, this);
// 	EventLoop::addHandler(client);
// 	return client;
// }

Client *VirtualServer::createClient(int clientFd)
{
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "Failed to set client socket to non-blocking mode: " << strerror(errno) << std::endl;
		close(clientFd);
		return NULL;
	}
	Client *client = new Client(clientFd, this);
	EventLoop::addHandler(client);
	return client;
}





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