#include "HttpResponse.hpp"
#include <sstream>
#include <sys/socket.h>

HttpResponse::HttpResponse()
{
    m_statusCode = 200;
    m_body = "Hello, World!";
    m_protocol = "HTTP/1.1";
    m_headers["Content-Type"] = "text/plain";
    m_headers["Content-Length"] = m_body.size();
    m_headers["Connection"] = "close";
}

HttpResponse::~HttpResponse()
{
}


// ANCHOR Setters
void HttpResponse::setBody(const std::string& body)
{
    m_headers["Content-Length"] = m_body.size();
    m_body = body;
}
void HttpResponse::setStatusCode(int statusCode)
{
    m_statusCode = statusCode;
}
void HttpResponse::setProtocol(const std::string& protocol)
{
    m_protocol = protocol;
}
void HttpResponse::setHeader(const std::string& key, const std::string& value)
{
    m_headers[key] = value;
}




// ANCHOR Getters
std::string HttpResponse::getBody() const
{
    return m_body;
}
int HttpResponse::getStatusCode() const
{
    return m_statusCode;
}
std::string HttpResponse::getProtocol() const
{
    return m_protocol;
}
std::string HttpResponse::getHeader(const std::string& key) const
{
    std::map<std::string, std::string>::const_iterator it = m_headers.find(key);
    if (it != m_headers.end())
        return it->second;
    return "";
}

// ANCHOR Send response
void HttpResponse::sendResponse(int clientSocket)
{
    std::ostringstream responseStream;
    responseStream << m_protocol << " " << m_statusCode << " OK\r\n";
    std::map<std::string, std::string>::const_iterator it = m_headers.begin();
    for (; it != m_headers.end(); ++it)
        responseStream << it->first << ": " << it->second << "\r\n";
    responseStream << "\r\n";
    
    responseStream << m_body;
    std::string response = responseStream.str();
    send(clientSocket, response.c_str(), response.size(), 0);
}