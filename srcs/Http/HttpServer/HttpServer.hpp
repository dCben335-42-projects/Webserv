#ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

#include <string>
#include "Url.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <sstream>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class HttpServer
{
    private:
        unsigned short      m_socket;
        unsigned short      m_port;
        struct sockaddr_in  m_address;

        void                handleClient(int clientSocket); 
    public:
        HttpServer(unsigned short port = 9058);
        ~HttpServer();
        void run();
};

#endif
