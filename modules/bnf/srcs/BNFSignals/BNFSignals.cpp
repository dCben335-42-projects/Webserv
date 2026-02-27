#include "BNFSignals.hpp"

bool BNFSignals::m_end = false;

void BNFSignals::sendStop(int signum)
{
	(void) signum;
	m_end = true;
}

void BNFSignals::catchSigs()
{
	signal(SIGINT, BNFSignals::sendStop);
	signal(SIGQUIT, BNFSignals::sendStop);
	signal(SIGTERM, BNFSignals::sendStop);
}

bool BNFSignals::getEnd()
{
	return m_end;
}
