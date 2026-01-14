#ifndef CGI_SPAWNER_HPP
#define CGI_SPAWNER_HPP

#include "Client.hpp"
#include "VirtualServer.hpp"
#include <string>
#include <unistd.h>
#include <sys/types.h>

class CgiSpawner
{
	private:
		Client		  *m_client;
		VirtualServer *m_server;
		int			   m_stdin_pipe[2];
		int			   m_stdout_pipe[2];
		pid_t		   m_pid;
		bool		   m_has_post_data;

		void setupPipes();
		void setupEnv(const std::string &script_path);
		void childprocess(const std::string &script_path);
		void parentProcess();
		void writePostData();
		void closeUnusedPipes();

	public:
		CgiSpawner(Client *client, VirtualServer *server);

		int spawn(const std::string &script_path);
};

#endif