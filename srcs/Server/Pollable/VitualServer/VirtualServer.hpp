#ifndef VIRTUAL_SERVER_HPP
#define VIRTUAL_SERVER_HPP

#include "Pollable.hpp"
#include "Client.hpp"
#include "Config.hpp"

class VirtualServer : public Pollable
{
	private:
		const Config  &m_config;
		std::string	   m_bindIp;
		unsigned short m_bindPort;

		bool setupSocket(const std::string &ip, unsigned short port);

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
};

#endif
