#include "HttpServer.hpp"
#include "HttpResponse.hpp"
#include "config.hpp"

#include <string> 
#include <cstring>
#include <cctype>    
#include <stdexcept> 
#include <iostream>  
#include <cstdlib>


HttpServer::HttpServer(unsigned short port)
    : m_socket(-1), m_port(port)
{

    // SOCK_STREAM TCP
    m_socket = socket(SOCKET_IPV4, SOCKET_TCP, SOCKET_PROTOCOL);
    if (m_socket < 0)
        throw std::runtime_error("Failed to create socket");

    int opt = 1;

    // SOL_SOCKET is the socket level itself which allows us to set options
    // SO_REUSEADDR allows the socket to be bound to an address that is already in use
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (opt < 0)
        throw std::runtime_error("setsockopt failed");
    std::memset(&m_address, 0, sizeof(m_address));
    m_address.sin_family = SOCKET_IPV4;
    m_address.sin_addr.s_addr = SOCKET_INTERFACE;
    m_address.sin_port = htons(m_port);

    if (bind(m_socket, (struct sockaddr *)&m_address, sizeof(m_address)) < 0)
        throw std::runtime_error("Bind failed");
    if (listen(m_socket, SOMAXCONN) < 0)
        throw std::runtime_error("Listen failed");
    std::cout << "Server started on port " << "http://localhost:" << m_port << std::endl;
}

HttpServer::~HttpServer()
{
    if (m_socket > 0)
        close(m_socket);
}


// In your HttpServer class
void HttpServer::handleClient(int clientSocket)
{
    HttpResponse    res;

    try
    {
        HttpRequest req = HttpRequest().read(clientSocket).parse();
        std::cout << "\n----- Request Start -----" << std::endl;
        std::cout << req << std::endl;

        if (req.getHeader("Host").empty())
        {
            throw std::runtime_error("400 Bad Request: Missing Host header");
        }

        std::cout << "======> Response " << std::endl;
        res.setStatusCode(200);
        res.setHeader(HEADER_CONTENT_TYPE, HEADER_TYPES_HTML);
        res.setBody("<html><body><h1>Hello, World!</h1></body></html>");
        res.sendResponse(clientSocket);
        std::cout << res << std::endl;
        std::cout << "----- Request End -----\n" << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        res.setStatusCode(501);
        res.setHeader(HEADER_CONTENT_TYPE, HEADER_TYPES_JSON);
        res.setBody("{\n\"error\": \"" + std::string(e.what()) + "\"\n}");
        
        std::cout << res << std::endl;
        res.sendResponse(clientSocket);
    }
    close(clientSocket);
    
  
}


void HttpServer::run()
{
    struct sockaddr_in      clientAddr;
    socklen_t               clientLen = sizeof(clientAddr);
    std::string             request;
    int clientSocket;


    while (true)
    {
        clientSocket = accept(m_socket, (struct sockaddr *)&clientAddr, &clientLen);
        if (clientSocket < 0)
            throw std::runtime_error("Accept failed");
        handleClient(clientSocket);
    }
}