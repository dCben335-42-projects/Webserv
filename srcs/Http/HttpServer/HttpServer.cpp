#include "HttpServer.hpp"
#include "HttpResponse.hpp"
#include "config.hpp"

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

    if (bind(m_socket, (struct sockaddr*)&m_address, sizeof(m_address)) < 0)
        throw std::runtime_error("Bind failed");
    if (listen(m_socket, SOMAXCONN) < 0)
        throw std::runtime_error("Listen failed");
    std::cout << "Server started on port " << m_port << std::endl;
}

HttpServer::~HttpServer() {
    if (m_socket > 0)
        close(m_socket);
}

void HttpServer::run() {
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    while (true)
    {
        int clientSocket = accept(m_socket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            throw std::runtime_error("Accept failed");
        }

        char buffer[2048];
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead < 0)
        {
            HttpResponse res;

            res.setStatusCode(500);
            res.setBody("Internal Server Error");
            res.setHeader("Content-Type", "text/html");
            res.sendResponse(clientSocket);
            close(clientSocket);
            throw std::runtime_error("Receive failed");
        }
        buffer[bytesRead] = '\0';
        std::cout << "Received request:\n" << buffer << std::endl;
        HttpResponse res;
        res.setBody("Hello from HttpServer!");
        res.setHeader("Content-Type", "text/html");
        res.sendResponse(clientSocket);
        std::cout << "Response Header :" << res.getHeader("Content-Type") << std::endl;
        close(clientSocket);
    }
}