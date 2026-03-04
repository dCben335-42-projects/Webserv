#ifndef EARLY_SIGNAL_HPP
#define EARLY_SIGNAL_HPP

#include <csignal>

class EarlySignal
{
	private:
		bool				m_caught;
		static EarlySignal *m_instance;

		static void staticHandler(int signum);
		void		handler(int signum);

	public:
		EarlySignal();
		~EarlySignal();

		bool getCaught() const;
		void setCaught(bool caught);
};
#endif