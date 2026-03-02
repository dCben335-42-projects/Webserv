#ifndef BNF_SIGNALS_HPP
#define BNF_SIGNALS_HPP

#include <signal.h>

class BNFSignals
{
	private:
		static bool m_end;

		static void sendStop(int signum);

	public:
		static bool getEnd();
		static void catchSigs();
};

#endif