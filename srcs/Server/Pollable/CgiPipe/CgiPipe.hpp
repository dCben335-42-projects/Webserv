#ifndef CGI_PIPE_HPP
#define CGI_PIPE_HPP

#include "Pollable.hpp"
#include "Client.hpp"
#include <string>
#include <sys/types.h>

class CgiPipe : public Pollable
{
	private:
		Client	   *m_client;
		pid_t		m_pid;
		std::string m_scriptPath;

		void onError();
		void waitChild();

	public:
		CgiPipe(
			pid_t			   pid,
			int				   pipe_fd,
			Client			  *client,
			const std::string &scriptPath);
		~CgiPipe();

		void onPollIn();
		void onPollOut();
		void onPollErr();
};



#endif