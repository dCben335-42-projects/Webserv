#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <netinet/in.h>
#include <poll.h>

#include "Map.hpp"
#include "Vector.hpp"

#include "HttpClient.hpp"

class HttpServer
{
	private:
		Vec<int>			 m_hosts;
		Map<int, HttpClient> m_clients;
		Vec<pollfd>			 m_pollFds;

		void createPoll(const int socket);
		void createPollClient(const int serverFd);
		void createPollServer(const unsigned short port);

		void updatePollStatus();
		void deleteClient(const int &clientFd, const size_t &index);

	public:
		HttpServer();
		~HttpServer();

		void run();
};

#endif
