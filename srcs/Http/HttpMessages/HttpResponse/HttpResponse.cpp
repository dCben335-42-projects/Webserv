#include "HttpResponse.hpp"
#include <sstream>
#include <sys/socket.h>
#include "config.hpp"
#include <iostream>
#include "Number.hpp"

struct StatusCodeReason {
    unsigned short code;
    std::string reason;
};

const StatusCodeReason g_statusCodes[] = {
    {200,   "OK"},
    {201,   "Created"},
    {204,   "No Content"},
    {400,   "Bad Request"},
    {401,   "Unauthorized"},
    {403,   "Forbidden"},
    {404,   "Not Found"},
    {405,   "Method Not Allowed"},
    {413,   "Payload Too Large"},
    {414,   "URI Too Long"},
    {500,   "Internal Server Error"},
    {501,   "Not Implemented"},
    {0,     "Unknown Status"}
};

HttpResponse::HttpResponse(): AHttpMessage(GET)
{
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
    std::string         reason;
    size_t              i = 0;
    
    head << PROTOCOL << " " << m_statusCode << " ";
    while (g_statusCodes[i].code != 0)
    {
        if (g_statusCodes[i].code == m_statusCode)
        {
            reason = g_statusCodes[i].reason;
            break;
        }
        ++i;
    }
    if (reason.empty())
        reason = g_statusCodes[i].reason;
    head << reason << HEADER_LINE_ENDING;
    return head.str();
}


// ANCHOR Send response
void HttpResponse::sendResponse(int clientSocket)
{
    std::ostringstream responseStream;
    responseStream << getHead();
    std::map<std::string, std::string>::const_iterator it = m_headers.begin();
    for (; it != m_headers.end(); ++it)
        responseStream << it->first << ": " << it->second << HEADER_LINE_ENDING;
    responseStream << HEADER_LINE_ENDING;
    responseStream << m_body;
    std::string response = responseStream.str();
    send(clientSocket, response.c_str(), response.size(), 0);
}



// ANCHOR Overload operator <<
std::ostream &operator<<(std::ostream &o, HttpResponse const &response)
{
    o << response.getHead() << static_cast<AHttpMessage const&>(response);
    o << "\n"<< "Body:" << "\n" << response.getBody();
    return o;
}
