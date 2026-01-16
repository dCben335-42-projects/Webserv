#ifndef VIRTUAL_SERVER_HPP
#define VIRTUAL_SERVER_HPP

#include "Pollable.hpp"
#include "Client.hpp"
#include <string>
#include <netinet/in.h>

class VirtualServer : public Pollable
{
	public:
		struct Config
		{
				std::string root;
				std::string cgiBin;
				std::string indexFile;

				std::string	   ip;
				unsigned short port;
				std::string	   domain;
		};

	private:
		bool		   setupSocket(const std::string &ip, unsigned short port);
		std::string	   m_bindIp;
		unsigned short m_bindPort;
		Config		   m_config;

	public:
		VirtualServer(const std::string &ip, unsigned short port, const Config &config);
		~VirtualServer();

		Client *createClient(int clientFd);

		std::string	   getBindIp() const;
		unsigned short getBindPort() const;
		std::string	   getDocumentRoot() const;
		std::string	   getCgiBin() const;
		std::string	   getIndexFile() const;

		void onPollIn();
		void onPollOut();
		void onPollErr();
		void onPollHangUp();
};

#endif