#ifndef CGI_PIPE_HPP
#define CGI_PIPE_HPP

#include "Pollable.hpp"
#include "Client.hpp"
#include "CgiOutput.hpp"
#include <sys/types.h>

class CgiPipe : public Pollable
{
	private:
		Client *const m_client;
		CgiOutput	  m_cgiOutput;
		pid_t		  m_pid;

		void onError();
		void waitChild();

	public:
		CgiPipe(
			pid_t		  pid,
			int			  pipe_fd,
			Client *const client);
		~CgiPipe();

		void onPollIn();
		void onPollOut();
		void onPollErr();
		void onPollHangUp();
};



#endif