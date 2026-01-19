#ifndef VIRTUAL_SERVER_HPP
#define VIRTUAL_SERVER_HPP

#include "Pollable.hpp"
#include "Client.hpp"
#include <string>
#include <netinet/in.h>
#include "Config.hpp"


class VirtualServer : public Pollable
{
	private:
		bool		   setupSocket(const std::string &ip, unsigned short port);
		std::string	   m_bindIp;
		unsigned short m_bindPort;
		Config		   m_config;

	public:
		VirtualServer(const Config &config);
		~VirtualServer();

		Client *createClient(int clientFd);

		std::string	   getBindIp() const;
		unsigned short getBindPort() const;

		void onPollIn();
		void onPollOut();
		void onPollErr();
		void onPollHangUp();
};

#endif