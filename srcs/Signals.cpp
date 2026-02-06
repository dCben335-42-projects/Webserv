#include "Signals.hpp"
#include "EventLoop.hpp"

bool Signals::m_isChild = false;

void Signals::sendStop(int signum)
{
	(void) signum;
	EventLoop::destroy();
	if (!m_isChild)
		exit(0);
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

void Signals::setIsChild(bool isChild)
{
	m_isChild = isChild;
}