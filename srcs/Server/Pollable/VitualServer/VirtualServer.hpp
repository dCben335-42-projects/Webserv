#ifndef VIRTUAL_SERVER_HPP
#define VIRTUAL_SERVER_HPP

#include "Pollable.hpp"
#include "Config.hpp"
#include <netinet/in.h>
#include "SocketAddress.hpp"

class Client;
class HttpUser;

class VirtualServer : public Pollable
{
	private:
		const Config		m_defaultConf;
		const ServerConfigs m_configs;
		const SocketAddress m_bindAddress;

		Vec<HttpUser> m_users;

		bool setupSocket();

		void setReusable(int sockFd);
		void bindAndListen(int sockFd);
		void createClient(int clientFd);

		typedef Vec<HttpUser>::iterator		  UserIt;
		typedef Vec<HttpUser>::const_iterator UserCIt;

	public:
		VirtualServer(
			const SocketAddress &address,
			const Config		&http,
			const ServerConfigs &serverConfigs);
		~VirtualServer();

		const Config		&getConfig() const;
		const ServerConfigs &getConfigs() const;

		void onPollIn();
		void onPollOut();
		void onPollErr();
		void onPollHangUp();

		bool isTimedOut(const time_t currentTime) const;
		void onTimeOut();

		const std::string  createUser(const std::string name, const std::string pswd);
		bool			   hasUser(const std::string &hash) const;
		const std::string &findUser(const std::string &name, const std::string &pswd, const std::string &authLoc);


	private:
		class ServerException : public std::exception
		{
			protected:
				std::string m_message;

			public:
				ServerException(const std::string &msg) : m_message(msg)
				{
				}

				virtual ~ServerException() throw()
				{
				}

				virtual const char *what() const throw()
				{
					return m_message.c_str();
				}
		};

		class SocketCreationException : public ServerException
		{
			public:
				SocketCreationException() : ServerException("Socket creation failed")
				{
				}
		};

		class SocketOptionException : public ServerException
		{
			public:
				SocketOptionException() : ServerException("setsockopt failed")
				{
				}
		};

		class BindException : public ServerException
		{
			public:
				BindException() : ServerException("Bind failed")
				{
				}
		};

		class ListenException : public ServerException
		{
			public:
				ListenException() : ServerException("Listen failed")
				{
				}
		};
};

#endif
