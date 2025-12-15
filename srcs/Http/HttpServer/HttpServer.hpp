#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <netinet/in.h>
#include <poll.h>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
// #include "httpErrors.hpp"
#include "HttpClient.hpp"

#include "Map.hpp"
#include "Vector.hpp"

class HttpServer
{
	private:
		Vec<int>			 m_hosts;
		Vec<pollfd>			 m_pollFds;
		Map<int, HttpClient> m_clients;
		int					 m_serverFd;

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
