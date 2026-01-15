#ifndef POLLABLE_HPP
#define POLLABLE_HPP

#include <poll.h>
#include <iostream>

/**
 *
 * @class Pollable
 *
 * @brief Represents an entity that can be polled for events.
 *
 * This class encapsulates a file descriptor along with the events
 * to monitor and the events that have occurred.
 *
 */
class Pollable
{
	protected:
		int	  m_fd;
		short m_events;
		short m_revents;

		Pollable(int f);

	public:
		virtual ~Pollable()
		{
		}
		int	  getFd() const;
		short getEvents() const;
		short getRevents() const;

		void setEvents(short events);
		void setRevents(short revents);

		virtual void onPollIn()	 = 0;
		virtual void onPollOut() = 0;
		virtual void onPollErr()
		{
			std::cerr << "Pollable error on fd " << m_fd << std::endl;
		}
};

#endif