#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include "Pollable.hpp"
#include "Vec.hpp"
#include "Map.hpp"

class EventLoop
{
	private:
		static Vec<int>				m_hosts;
		static Vec<pollfd>			m_pollFds;
		static Map<int, Pollable *> m_handlers;
		static size_t				m_nfds;

		static void		 deletePollable(const int fd, const size_t &index);
		static Pollable *getHandler(const int fd);

	public:
		static const size_t maxFds = 4096;

		static void init();
		static void run();
		static void destroy();

		static void addHandler(Pollable *const handler);
		static void updatePollStatus();

		static size_t getNfds();
};



#endif
