#include "HttpResponse.hpp"
#include <sstream>
#include <sys/socket.h>
#include "config.hpp"

HttpResponse::HttpResponse(): AHttpMessage(GET)
{
    m_statusCode = 200;
    // m_body = "Hello, World!";
    m_headers["Content-Type"] = "application/json";
    // m_headers["Content-Length"] = m_body.size();
    // m_headers["Connection"] = "close";
}

HttpResponse::~HttpResponse()
{
}


// ANCHOR Setters
void HttpResponse::setStatusCode(unsigned short code) {
    m_statusCode = code;   
}

// ANCHOR Getters
unsigned short  HttpResponse::getStatusCode() const {
    return m_statusCode;
}

std::string HttpResponse::getHead() const
{
    std::ostringstream  head;

    head << PROTOCOL << " " 
         << m_statusCode << " "
         << (m_statusCode >= 400 ? "KO" : "OK") 
         << "\r\n";
    return head.str();
}


// ANCHOR Send response
void HttpResponse::sendResponse(int clientSocket)
{
    std::ostringstream responseStream;
    responseStream << getHead();
    std::map<std::string, std::string>::const_iterator it = m_headers.begin();
    for (; it != m_headers.end(); ++it)
        responseStream << it->first << ": " << it->second << "\r\n";
    responseStream << "\r\n";
    responseStream << m_body;
    std::string response = responseStream.str();
    send(clientSocket, response.c_str(), response.size(), 0);
}



#include <ostream>

std::ostream &operator<<(std::ostream &o, HttpResponse const &response)
{
    o << response.getHead() << static_cast<AHttpMessage const&>(response);
    return o;
}
