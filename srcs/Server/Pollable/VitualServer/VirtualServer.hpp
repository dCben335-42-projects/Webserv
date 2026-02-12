#ifndef VIRTUAL_SERVER_HPP
#define VIRTUAL_SERVER_HPP

#include "Pollable.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "HttpUser.hpp"

class VirtualServer : public Pollable
{
	private:
		const Config  &m_config;
		std::string	   m_bindIp;
		unsigned short m_bindPort;
		Vec<HttpUser>  m_users;

		bool setupSocket(const std::string &ip, unsigned short port);

		typedef Vec<HttpUser>::iterator		  UserIt;
		typedef Vec<HttpUser>::const_iterator UserCIt;

	public:
		VirtualServer(const Config &config);
		~VirtualServer();

		Client *createClient(int clientFd);

		const Config  &getConfig() const;
		std::string	   getBindIp() const;
		unsigned short getBindPort() const;

		void onPollIn();
		void onPollOut();
		void onPollErr();
		void onPollHangUp();

		bool isTimedOut(const time_t currentTime) const;
		void onTimeOut();

		const std::string  createCookie(const std::string name, const std::string pswd);
		bool			   hasUser(const std::string &hash) const;
		const std::string &findCookie(const std::string &name, const std::string &pswd);
};

#endif
