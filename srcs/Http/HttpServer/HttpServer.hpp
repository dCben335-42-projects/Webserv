#ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

#include <string>
#include "Url.hpp"s

class HttpServer
{
    private:
        int             m_port;
        std::string     m_address;
        Url<>   m_url;
        
    public:
        HttpServer();
        ~HttpServer();
};

HttpServer::HttpServer(/* args */)
{
}

HttpServer::~HttpServer()
{
}
