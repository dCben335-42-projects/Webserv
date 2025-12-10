# include "HttpServer.hpp"
# include "config.hpp"
# include <unistd.h>
# include <cstring>
# include <stdexcept> 
# include "FileHandler.hpp"
# include "httpErrors.hpp"

HttpServer::HttpServer(unsigned short port): m_socket(-1), m_port(port)
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
    createPoll(m_socket);
}

HttpServer::~HttpServer()
{
    if (m_socket > 0)
        close(m_socket);
}


void HttpServer::respond(HttpRequest &req, HttpResponse &res)
{
    const std::string ROOT = "files/www";
    try
    {
        req.read().parse();
        std::cout << "\n----- Request Start -----" << std::endl;
        std::cout << req << std::endl;

        std::cout << "\n\n Raw request: \n"<<req.getRawRequest() << "\n"<< std::endl;
        std::cout << "======> Response " << std::endl;        
        try
        {
            if (req.getTarget() == "/" && req.getHeader("Host") == "localhost:8080")
            {
                res.setBody(FileHandler::readFile(ROOT + "/index.html"));
                res.setHeader(HEADER_CONTENT_TYPE, HEADER_TYPES_HTML);
            }
            else
            {
                std::string safePath = FileHandler::getSafePath(ROOT, req.getTarget().substr(1));
                res.setBody(FileHandler::readFile(safePath));
                if (safePath.find(".html") != std::string::npos)
                    res.setHeader(HEADER_CONTENT_TYPE, HEADER_TYPES_HTML);
                else if (safePath.find(".css") != std::string::npos)
                    res.setHeader(HEADER_CONTENT_TYPE, HEADER_TYPES_CSS);
                else if (safePath.find(".js") != std::string::npos)
                    res.setHeader(HEADER_CONTENT_TYPE, HEADER_TYPES_JS);
                else
                    res.setHeader(HEADER_CONTENT_TYPE, HEADER_TYPES_PLAIN);
            }
        }
        catch(const std::exception& e)
        {
            std::cerr <<  RED << e.what() << RESET << '\n';
            throw Http404Error();
        }
        res.setStatusCode(200);
        res.sendRes();
        std::cout << res << std::endl;
        std::cout << "----- Request End -----\n" << std::endl;
    }
    catch(const AHttpError &e)
    {
        res.setStatusCode(e.getStatusCode());
        res.setHeader(HEADER_CONTENT_TYPE, HEADER_TYPES_JSON);
        res.setBody("{\n\t\"status\": \"error\",\n\t\"message\": \"" + std::string(e.what()) + "\"\n}");
        std::cout << res << std::endl;
        res.sendRes();
    }
}

void HttpServer::createPoll(int socket)
{
    pollfd  serverFd;

    serverFd.fd = socket;
    serverFd.events = POLLIN;
    m_pollFds.push_back(serverFd);
}


void HttpServer::createPollClient(void)
{   
    struct sockaddr_in  clientAddr;
    socklen_t           clientLen = sizeof(clientAddr);
    int                 clientSocket;

    try {
        clientSocket = accept(m_socket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0)
            throw std::runtime_error("Accept failed");
        createPoll(clientSocket);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

    
void HttpServer::handleClient(int clientSocket)
{
    HttpRequest req(clientSocket);
    HttpResponse res(clientSocket);
    respond(req, res);
}

void HttpServer::run()
{
    int     pollCount;
    int     clientSocket;

    while (true)
    {
        pollCount = poll(m_pollFds.data(), m_pollFds.size(), -1);
        if (pollCount < 0)
            throw std::runtime_error("Poll failed");
        for (size_t i = 0; i < m_pollFds.size() && pollCount > 0; ++i)
        {
            if (!(m_pollFds[i].revents & POLLIN))
                continue;
            pollCount--;
            if (m_pollFds[i].fd != m_socket)
            {
                clientSocket = m_pollFds[i].fd;
                handleClient(clientSocket);
                close(clientSocket);
                m_pollFds.erase(m_pollFds.begin() + i);
                i--;
            }
            else
                createPollClient();
        }
    }
}
