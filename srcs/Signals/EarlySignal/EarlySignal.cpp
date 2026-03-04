#include "EarlySignal.hpp"


EarlySignal *EarlySignal::m_instance = NULL;

EarlySignal::EarlySignal() : m_caught(false)
{
	m_instance = this;
	signal(SIGINT, EarlySignal::staticHandler);
	signal(SIGTERM, EarlySignal::staticHandler);
	signal(SIGQUIT, EarlySignal::staticHandler);
}


EarlySignal::~EarlySignal()
{
	m_instance = NULL;
}



/**
 *
 * @brief Static signal handler function that sets the caught status when a signal is received.
 *
 * @param signum The signal number that was received.
 *
 */
void EarlySignal::staticHandler(int signum)
{
	(void) signum;
	if (m_instance)
		m_instance->setCaught(true);
}



/**
 *
 * @brief Retrieves the caught status of the signal.
 *
 * @return A boolean value indicating whether the signal was caught or not.
 *
 */
bool EarlySignal::getCaught() const
{
	return m_caught;
}


/**
 *
 * @brief Sets the caught status of the signal.
 *
 * @param caught A boolean value indicating whether the signal was caught or not.
 *
 */
void EarlySignal::setCaught(bool caught)
{
	m_caught = caught;
}