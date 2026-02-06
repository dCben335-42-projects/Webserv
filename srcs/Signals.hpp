#ifndef SIGNALS_HPP
#define SIGNALS_HPP

#include <signal.h>

class Signals
{
	private:
		static bool m_isChild;

		static void sendStop(int signum);

	public:
		static bool getIsChild();
		static void setIsChild(bool isChild);
		static void catchSigs();
};

#endif