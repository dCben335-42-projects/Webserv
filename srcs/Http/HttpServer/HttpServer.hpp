#ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

#include <netinet/in.h>
#include <poll.h>
#include <vector>

# include "HttpRequest.hpp"
# include "HttpResponse.hpp"
// # include "httpErrors.hpp" 
# include "HttpClient.hpp"

class HttpServer
{
	private:
		unsigned short			m_socket;
		unsigned short			m_port;
		struct sockaddr_in		m_address;


		std::vector<pollfd>		m_pollFds;
		Map<int, HttpClient>	m_clients;

		void	createPoll(int socket);
		void	respond(HttpRequest &req, HttpResponse &res); 
		void	createPollClient(void);
	public:
		HttpServer(unsigned short port);
		~HttpServer();

		void run();
};

#endif
