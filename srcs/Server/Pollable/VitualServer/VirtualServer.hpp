#ifndef VIRTUAL_SERVER_HPP
#define VIRTUAL_SERVER_HPP

#include "Pollable.hpp"
#include "Config.hpp"
#include <netinet/in.h>

class Client;
class HttpUser;

class VirtualServer : public Pollable
{
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


	private:
		const Config &m_config;

		const Vec<const Config *> configs;

		bool		   m_isIPv6;
		std::string	   m_bindIp;
		unsigned short m_bindPort;
		Vec<HttpUser>  m_users;

		bool setupSocket(const std::string &ip, unsigned short port);

		uint32_t convertIPv4(const std::string &ip);
		uint32_t resolveIPv4(const std::string &hostname);

		struct in6_addr convertIPv6(const std::string &ip);
		struct in6_addr resolveIPv6(const std::string &hostname);

		void setReusable(int sockFd);
		void bindAndListen(int sockFd, const std::string &ip, unsigned short port);
		void createClient(int clientFd);

		typedef Vec<HttpUser>::iterator		  UserIt;
		typedef Vec<HttpUser>::const_iterator UserCIt;

	public:
		VirtualServer(const Config &config);
		~VirtualServer();

		const Config  &getConfig() const;
		std::string	   getBindIp() const;
		unsigned short getBindPort() const;

		const Vec<const Config *> &getConfigs() const;

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
