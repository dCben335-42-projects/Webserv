#ifndef CGI_SPAWNER_HPP
#define CGI_SPAWNER_HPP

#include "Client.hpp"
#include "VirtualServer.hpp"
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include "Vec.hpp"

class CgiSpawner
{
	private:
		const HttpRequest &m_req;
		const Config	  &m_config;

		std::string		 m_binPath;
		std::string		 m_scriptPath;
		std::string		 m_pathInfo;
		std::string		 m_scriptName;
		Map<std::string> m_activeBinMap;


		int m_stdinPipe[2];
		int m_stdoutPipe[2];

		pid_t m_pid;

		bool			 m_has_post_data;
		Vec<std::string> m_env;
		char			*m_envp[32];

		void		setupPipes();
		std::string makeEnvVar(const std::string &key, const std::string &value);
		void		generateEnvp();
		void		setupEnv();
		void		childprocess();

		ssize_t getScriptIndex(const Vec<std::string> &urlSegments, const size_t start);

		void buildPathInfo(const Vec<std::string> &urlSegments, ssize_t scriptIndex);
		void buildScriptName(const Vec<std::string> &urlSegments, ssize_t scriptIndex);


	public:
		CgiSpawner(const HttpRequest &req, const Config &config);
		~CgiSpawner();

		void spawn();
		void spawnCgi(Client *const client);

		bool isCgi();
};

#endif