#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include "Pollable.hpp"
#include "Vec.hpp"
#include "Map.hpp"
#include "Config.hpp"

class EventLoop
{
	private:
		static Vec<int>				m_hosts;
		static Vec<pollfd>			m_pollFds;
		static Map<int, Pollable *> m_handlers;
		static size_t				m_nfds;
		static bool					m_isRunning;

		static void		 deletePollable(const int fd, const size_t &index);
		static Pollable *getHandler(const int fd);

		static void updatePollStatus();


	public:
		static void init(const Config &httpConfig, const VServersConfigs &serversConfigs);
		static void run();
		static void destroy();

		static void	  addHandler(Pollable *const handler);
		static size_t getNfds();
		static void	  setIsRunning(bool isRunning);
};



#endif
