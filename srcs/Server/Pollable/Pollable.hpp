#ifndef POLLABLE_HPP
#define POLLABLE_HPP

#include <poll.h>

/**
 *
 * @class Pollable
 *
 * @brief Represents an entity that can be polled for events.
 * This class encapsulates a file descriptor along with the events
 * to monitor and the events that have occurred.
 *
 */
class Pollable
{
	protected:
		/**
		 * @var m_fd The file descriptor associated with this Pollable.
		 * @brief The file descriptor to monitor for events.
		 */
		int m_fd;

		short m_events;
		short m_revents;
		bool  m_connectionClosed;

		Pollable(int f);

	public:
		virtual ~Pollable()
		{
		}
		int	  getFd() const;
		short getEvents() const;
		short getRevents() const;
		bool  getConnectionClosed() const;

		void setEvents(short events);
		void setRevents(short revents);
		void setConnectionClosed(bool isClosed);

		virtual void onPollIn()		= 0;
		virtual void onPollOut()	= 0;
		virtual void onPollHangUp() = 0;
		virtual void onPollErr()
		{
			(void) 0;
		}

		virtual bool isTimedOut(const time_t currentTime) const = 0;
};

#endif
