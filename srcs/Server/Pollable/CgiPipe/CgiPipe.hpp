#ifndef CGI_PIPE_HPP
#define CGI_PIPE_HPP

#include "Pollable.hpp"
#include "Client.hpp"
#include "CgiOutput.hpp"
#include <sys/types.h>

#define LOG_MSG_CGIPIPE_PREFIX "CGIPipe - "
#define LOG_MSG_PIPE_CLOSED LOG_MSG_CGIPIPE_PREFIX "Pipe closed from the other end, stream closed"
#define ERROR_MSG_CGI_TIMEOUT LOG_MSG_CGIPIPE_PREFIX "CGI timed out"

class CgiPipe : public Pollable
{
	private:
		Client *const m_client;
		CgiOutput	  m_cgiOutput;
		pid_t		  m_pid;
		size_t		  m_bytesWritten;

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

		bool isTimedOut(const time_t currentTime) const;
		void onTimeOut();
};



#endif