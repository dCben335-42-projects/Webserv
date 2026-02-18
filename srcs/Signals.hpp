#ifndef SIGNALS_HPP
#define SIGNALS_HPP

#include <signal.h>

class Signals
{
	private:
		static bool m_isChild;
		static bool m_isDestroyed;

		static void sendStop(int signum);

	public:
		static bool getIsChild();
		static bool getIsDestroyed();

		static void setIsChild(bool isChild);
		static void setIsDestroyed(bool isDestroyed);

		static void catchSigs();
};

#endif