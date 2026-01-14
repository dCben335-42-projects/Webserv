#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include "Pollable.hpp"
#include "Vector.hpp"
#include "Map.hpp"
#include <poll.h>

class EventLoop
{
	public:
		static const size_t maxFds = 4096;

		static void init();
		static void run();
		static void addHandler(Pollable *handler);
		static void removeHandler(size_t index);
		static int	getNfds()
		{
			return nfds;
		}
		static void updatePollStatus();


	private:
		static Vec<int>				m_hosts;
		static Vec<pollfd>			m_pollFds;
		static Map<int, Pollable *> m_handlers;
		static int					nfds;

		static void createPollServer(unsigned short port);
		static void createPoll(const int socket);
		static void createPollClient(const int serverFd);
		static void deletePollable(const int &fd, const size_t &index);

		static Pollable *getHandler(int fd);
};



#endif
