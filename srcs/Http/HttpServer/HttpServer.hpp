#ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

#include <netinet/in.h>
#include <poll.h>
#include <vector>

# include "HttpRequest.hpp"
# include "HttpResponse.hpp"
# include "httpErrors.hpp" 

class HttpServer
{
    private:
        unsigned short          m_socket;
        unsigned short          m_port;
        struct sockaddr_in      m_address;
        std::vector<pollfd>     m_pollFds;

        void	createPoll(int socket);
        void	respond(HttpRequest &req, HttpResponse &res); 
		void	handleClient(int clientSocket); 
        void	createPollClient(void);

    public:
        HttpServer(unsigned short port);
        ~HttpServer();

        void run();
};

#endif
