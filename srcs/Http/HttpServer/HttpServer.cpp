#include "HttpServer.hpp"
#include <iostream>

HttpServer::HttpServer() : m_url("ws://0.0.0.0:80")
{
    m_port = 80;
    m_address = "0.0.0.0";
    UrlScheme scheme = m_url.scheme();
    std::cout << "HttpServer initialized with URL: " << scheme << std::endl;

}

HttpServer::~HttpServer()
{
}