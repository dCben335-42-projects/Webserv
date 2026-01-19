// EventLoop.cpp
#include "EventLoop.hpp"
#include "VirtualServer.hpp"
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <fcntl.h>
#include <iostream>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Config.hpp"
#include "CgiParams.hpp"
#include "Location.hpp"

Map<int, Pollable *> EventLoop::m_handlers;
Vec<pollfd>			 EventLoop::m_pollFds;

void EventLoop::init()
{
	Map<std::string> binMap;
	binMap.set(".py", "/usr/bin/python3");
	binMap.set(".php", "/usr/bin/php");
	Config::CgiParams cgiParams(binMap);
	Config::Location *defaultLocation = new Config::Location(
		"/", "", "", false, &cgiParams);
	Config config1("127.0.0.1", 8080, "localhost", defaultLocation, Map<Config::Location *>());
	new VirtualServer(config1);
}

#include "Logger.hpp"

void EventLoop::run()
{
	const int POLL_TIMEOUT = 1000;

	while (true)
	{
		updatePollStatus();
		int ready = poll(&m_pollFds[0], m_pollFds.size(), POLL_TIMEOUT);
		if (ready < 0)
			break;
		for (size_t i = 0; i < m_pollFds.size() && ready > 0; ++i)
		{
			if (!m_pollFds[i].revents)
				continue;
			ready--;

			const int fd = m_pollFds[i].fd;
			// if (m_pollFds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
			// {
			// 	std::cerr << "QUI A LE PROBLEME DE FOU LÀ ?" << std::endl;
			// 	deletePollable(fd, i);
			// 	continue;
			// }

			Pollable *handler = getHandler(fd);
			if (handler)
			{
				handler->setRevents(m_pollFds[i].revents);

				if (m_pollFds[i].revents & POLLIN)
					handler->onPollIn();
				if (m_pollFds[i].revents & POLLOUT)
					handler->onPollOut();
				if (m_pollFds[i].revents & POLLERR)
					handler->onPollErr();
				if (m_pollFds[i].revents & POLLHUP)
					handler->onPollHangUp();
				if (handler->getConnectionClosed())
				{
					deletePollable(fd, i);
				}
				strerror(errno);
			}
		}
	}
}



void EventLoop::addHandler(Pollable *handler)
{
	if (!handler)
		return;
	const int	fd	   = handler->getFd();
	const short events = handler->getEvents();

	m_handlers.set(handler->getFd(), handler);
	pollfd pfd = {fd, events, 0};
	m_pollFds.push(pfd);
}


void EventLoop::deletePollable(const int &fd, const size_t &index)
{
	Pollable *handler = getHandler(fd);
	if (handler)
	{
		delete handler;
		m_handlers.remove(fd);
	}
	m_pollFds.remove(index);
	// close(fd);
}

void EventLoop::updatePollStatus()
{
	for (size_t i = 0; i < m_pollFds.size(); i++)
	{
		Pollable *handler = getHandler(m_pollFds[i].fd);
		if (handler)
			m_pollFds[i].events = handler->getEvents();
	}
}


Pollable *EventLoop::getHandler(int fd)
{
	Pollable *handler = NULL;
	m_handlers.try_get(fd, handler);
	return handler;
}
