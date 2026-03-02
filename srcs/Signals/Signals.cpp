#include "Signals.hpp"
#include "EventLoop.hpp"

bool Signals::m_isChild		= false;
bool Signals::m_isDestroyed = false;

void Signals::sendStop(int signum)
{
	(void) signum;
	if (m_isChild && !m_isDestroyed)
	{
		m_isDestroyed = true;
		EventLoop::destroy();
	}
	else
		EventLoop::setIsRunning(false);
}

void Signals::catchSigs()
{
	signal(SIGINT, Signals::sendStop);
	signal(SIGQUIT, Signals::sendStop);
	signal(SIGTERM, Signals::sendStop);
}

bool Signals::getIsChild()
{
	return m_isChild;
}

bool Signals::getIsDestroyed()
{
	return m_isDestroyed;
}

void Signals::setIsChild(bool isChild)
{
	m_isChild = isChild;
}

void Signals::setIsDestroyed(bool isDestroyed)
{
	m_isDestroyed = isDestroyed;
}