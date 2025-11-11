#include "HttpServer.hpp"
#include <iostream>

HttpServer::HttpServer() : m_url("https://0.0.0.0:80")
{
    m_port = 80;
    m_address = "0.0.0.0";
    std::cout << "HttpServer initialized with URL: " << m_url.getSchemeName() << std::endl;
    std::cout << "HttpServer initialized with URL: " << m_url.getProtocol() << std::endl;
}

HttpServer::~HttpServer()
{

}