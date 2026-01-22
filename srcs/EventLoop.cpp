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
#include "Location.hpp"
#include "Logger.hpp"
#include "CgiParams.hpp"

Map<int, Pollable *> EventLoop::m_handlers;
Vec<pollfd>			 EventLoop::m_pollFds;

void EventLoop::init()
{
	Map<std::string> binmap1;
	binmap1.set(".py", "/usr/bin/python3");
	binmap1.set(".php", "/usr/bin/php");
	Config::CgiParams *param2 = new Config::CgiParams(binmap1);

	Config::Location			 *defaultLocation = new Config::Location("", "files/www/website", "index.html", false, param2);
	Config::Location			 *loc1			  = new Config::Location("/assets", "", "", false, param2);
	Config::Location			 *loc2			  = new Config::Location("/assets/", "", "", false, NULL);
	Config::Location			 *loc3			  = new Config::Location("/assets/test/", "", "", false, NULL);
	Config::Location			 *loc4			  = new Config::Location("/assets/test/dwad", "", "", false, NULL);
	Config::Location			 *loc40			  = new Config::Location("/assets/test/dwad/", "", "", false, NULL);
	Config::Location			 *loc5			  = new Config::Location("/shrek", "", "", false, NULL);
	Config::Location			 *loc6			  = new Config::Location("/shrek/", "", "", false, NULL);
	Map<const Config::Location *> locations;
	locations.set(loc1->getPath(), loc1);
	locations.set(loc2->getPath(), loc2);
	locations.set(loc3->getPath(), loc3);
	locations.set(loc4->getPath(), loc4);
	locations.set(loc40->getPath(), loc40);
	locations.set(loc5->getPath(), loc5);
	locations.set(loc6->getPath(), loc6);
	Config *config1 = new Config("127.0.0.1", 8080, "localhost", defaultLocation, locations);
	new VirtualServer(*(config1));
}

void EventLoop::destroy()
{
	for (size_t i = 0; i < m_pollFds.size(); ++i)
	{
		const int fd = m_pollFds[i].fd;
		deletePollable(fd, i);
	}
	m_pollFds.clear();
	m_handlers.clear();
}


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
			const int fd	  = m_pollFds[i].fd;
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
					deletePollable(fd, i);
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
