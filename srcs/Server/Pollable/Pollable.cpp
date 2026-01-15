#include "Pollable.hpp"

Pollable::Pollable(int f) :
	m_fd(f),
	m_events(0),
	m_revents(0)
{
}




int Pollable::getFd() const
{
	return m_fd;
}
short Pollable::getEvents() const
{
	return m_events;
}
short Pollable::getRevents() const
{
	return m_revents;
}


void Pollable::setEvents(short events)
{
	m_events = events;
}
void Pollable::setRevents(short revents)
{
	m_revents = revents;
}