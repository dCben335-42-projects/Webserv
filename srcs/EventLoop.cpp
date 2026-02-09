#include "EventLoop.hpp"
#include "VirtualServer.hpp"
#include "Location.hpp"
#include "CgiParams.hpp"
#include "Signals.hpp"
#include "Logger.hpp"

Map<int, Pollable *> EventLoop::m_handlers;
Vec<pollfd>			 EventLoop::m_pollFds;
size_t				 EventLoop::m_nfds = 0;

void EventLoop::init()
{
	Map<std::string> binmap1;
	// Set<HttpMethod>	 allowedMethods1;
	// allowedMethods1.insert(HttpMethod::GET);
	// allowedMethods1.insert(HttpMethod::POST);
	// allowedMethods1.insert(HttpMethod::DELETE);
	// binmap1.set(".bla", "/home/tlutz/Documents/School/M5/Webserv/files/www/website/cgi_tester");
	binmap1.set(".py", "/usr/bin/python3");
	binmap1.set(".php", "/usr/bin/php");
	Config::CgiParams *param2 = new Config::CgiParams(binmap1);

	Config::Location *defaultLocation = new Config::Location("", "files/www/website", "", "index.htm", true, param2);
	Config::Location *loc1			  = new Config::Location("/assets", "", "", "", true, NULL);
	Config::Location *loc2			  = new Config::Location("/assets/", "", "files/www/website/assets", "", NULL);
	Config::Location *loc8			  = new Config::Location("/cgi-bin", "", "files/www/usr/bin", "", true, NULL);
	Config::Location *loc9			  = new Config::Location("/uploads", "", "files/www/uploads", "", true, NULL);
	loc9->setUpload("files/www/uploads/");
	Map<const Config::Location *> locations;
	locations.set(loc1->getPath(), loc1);
	locations.set(loc2->getPath(), loc2);
	locations.set(loc8->getPath(), loc8);
	locations.set(loc9->getPath(), loc9);
	Config *config1 = new Config("0.0.0.0", 8080, "localhost", defaultLocation, locations);
	new VirtualServer(*(config1));
	Signals::catchSigs();
}


/**
 *
 * @brief Destroys the event loop and cleans up resources.
 *
 * This function iterates through all registered poll file descriptors,
 * deletes their associated Pollable handlers, and clears the internal
 * data structures used for managing the event loop.
 *
 * @note It is meant to be called at the end of the program to free resources.
 * (when catching a termination signal for example)
 *
 */
void EventLoop::destroy()
{
	for (size_t i = 0; i < m_pollFds.size(); ++i)
	{
		const int fd = m_pollFds[i].fd;
		delete m_handlers.get(fd);
	}
	m_pollFds.clear();
	m_handlers.clear();
}


#include "Logger.hpp"
/**
 *
 * @brief Runs the event loop to handle I/O events.
 *
 * This function continuously monitors registered file descriptors for I/O events
 * using the poll() system call. It updates the poll status, processes events,
 * and invokes the appropriate handlers for each event.
 *
 * @note This function runs indefinitely until an error occurs in the poll() call.
 * @note It is the main loop of the event-driven architecture.
 * @note It can be stopped by external signals
 *
 */
void EventLoop::run()
{
	const int POLL_TIMEOUT = 1000;
	int		  ready;
	while (true)
	{
		updatePollStatus();
		ready = poll(&m_pollFds[0], m_pollFds.size(), POLL_TIMEOUT);
		if (ready < 0)
			break;
		for (size_t i = 0; i < m_pollFds.size() && ready > 0; ++i)
		{
			if (!m_pollFds[i].revents)
				continue;
			ready--;
			const int		fd		= m_pollFds[i].fd;
			Pollable *const handler = getHandler(fd);
			if (handler)
			{
				handler->setRevents(m_pollFds[i].revents);
				if (m_pollFds[i].revents & POLLHUP)
					handler->onPollHangUp();
				else if (m_pollFds[i].revents & POLLERR)
					handler->onPollErr();
				else if (m_pollFds[i].revents & POLLIN)
					handler->onPollIn();
				else if (m_pollFds[i].revents & POLLOUT)
					handler->onPollOut();
				if (handler->getConnectionClosed())
				{
					deletePollable(fd, i);
					i--;
				}
			}
		}
	}
}



/**
 *
 * @brief Adds a Pollable handler to the event loop.
 *
 * This function registers a new Pollable handler by adding it to the internal handler map
 * and creating a corresponding pollfd entry in the poll file descriptor vector.
 *
 * @param handler A pointer to the Pollable handler to be added.
 *
 */
void EventLoop::addHandler(Pollable *const handler)
{
	if (!handler)
		return;
	const int	fd	   = handler->getFd();
	const short events = handler->getEvents();

	m_handlers.set(handler->getFd(), handler);
	pollfd pfd = {fd, events, 0};
	m_pollFds.push(pfd);
	m_nfds++;
}



/**
 *
 * @brief Deletes a Pollable handler and its associated pollfd entry.
 *
 * This function removes the Pollable handler associated with the given file descriptor (fd)
 * from the internal handler map and deletes it. It also removes the corresponding pollfd
 * entry from the poll file descriptor vector at the specified index.
 *
 * @param fd The file descriptor of the Pollable handler to be deleted.
 * @param index The index of the pollfd entry in the poll file descriptor vector to be removed.
 *
 * @note It remains to each Pollable handler to close its own file descriptor in its destructor.
 *
 */
void EventLoop::deletePollable(const int fd, const size_t &index)
{
	Pollable *handler = getHandler(fd);
	if (handler)
	{
		delete handler;
		m_handlers.remove(fd);
	}
	m_pollFds.remove(index);
	m_nfds--;
}


/**
 *
 * @brief Updates the poll events for all registered Pollable handlers.
 *
 * This function iterates through all the poll file descriptors
 * and updates their event flags based on the current state
 * of their associated Pollable handlers.
 *
 * @note This function is actually meant to be called before each poll() call
 *
 */
void EventLoop::updatePollStatus()
{
	const time_t currentTime = std::time(0);
	for (size_t i = 0; i < m_pollFds.size(); i++)
	{
		Pollable *handler = getHandler(m_pollFds[i].fd);
		if (handler)
		{
			m_pollFds[i].events = handler->getEvents();
			if (handler->isTimedOut(currentTime))
			{
				Logger::info("Connection timed out on fd " + Logger::get(m_pollFds[i].fd));
				handler->setConnectionClosed(true);
				deletePollable(m_pollFds[i].fd, i);
				i--;
			}
		}
	}
}


/**
 *
 * @brief Retrieves the Pollable handler associated with the given file descriptor.
 *
 * @param fd The file descriptor for which to retrieve the handler.
 *
 * @return Either a VirtualServer, Client, or CgiPipe pointer associated with the fd,
 * or NULL if no handler is found.
 *
 */
Pollable *EventLoop::getHandler(int fd)
{
	Pollable *handler = m_handlers.get(fd);
	return handler;
}

/**
 *
 * @brief Retrieves the number of file descriptors currently managed by the event loop.
 *
 * @return The number of file descriptors (nfds).
 *
 */
size_t EventLoop::getNfds()
{
	return m_nfds;
}
