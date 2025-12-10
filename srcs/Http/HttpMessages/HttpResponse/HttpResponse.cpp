#include "HttpResponse.hpp"
#include <sstream>
#include <sys/socket.h>
#include "config.hpp"
#include <iostream>
#include "Number.hpp"

#include <cerrno>

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
    {405,   "Method Not Allowedsdawd"},

    {413,   "Payload Too Large"},
    {414,   "URI Too Long"},
    {500,   "Internal Server Error"},
    {501,   "Not Implemented"},
    {0,     "Unknown Status"}
};

HttpResponse::HttpResponse(): m_sentBytes(0) {}
HttpResponse::~HttpResponse() {}

// ANCHOR Reset
void HttpResponse::reset()
{
    m_statusCode = 0;
    m_sentBytes = 0;
    m_fullResponse.clear();
    m_headers.clear();
    m_body.clear();
    m_method = Method();
}

// ANCHOR Setters
void HttpResponse::setStatusCode(unsigned short code) {
    m_statusCode = code;   
}

// ANCHOR Getters
unsigned short  HttpResponse::getStatusCode() const {
    return m_statusCode;
}

std::string HttpResponse::getFullResponse() const {
    return m_fullResponse;
}

size_t HttpResponse::getSentBytes() const {
    return m_sentBytes;
}

// ANCHOR Build head
std::string HttpResponse::buildHead() const
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
void HttpResponse::build()
{
    std::ostringstream responseStream;
    responseStream << buildHead();
    std::map<std::string, std::string>::const_iterator it = m_headers.begin();
    for (; it != m_headers.end(); ++it)
        responseStream << it->first << ": " << it->second << HEADER_LINE_ENDING;
    responseStream << HEADER_LINE_ENDING;
    responseStream << m_body;
    m_fullResponse = responseStream.str();;
}

bool HttpResponse::sendRes(int socket)
{
	if (m_fullResponse.empty())
		return true;
	std::cout << "Sending response, total size: " << m_fullResponse.size() << " bytes." << std::endl;
    const char			*buf = m_fullResponse.c_str();
    const size_t		remaining = m_fullResponse.size() - m_sentBytes;
    const ssize_t		n = send(socket, buf + m_sentBytes, remaining, 0);

    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return false;
        throw std::runtime_error("send() failed");
    }
    m_sentBytes += n;
    return (m_sentBytes >= m_fullResponse.size());
}



// ANCHOR Overload operator <<
std::ostream &operator<<(std::ostream &o, HttpResponse const &response)
{
    o << response.buildHead() << static_cast<AHttpMessage const&>(response);
    o << "\n"<< "Body:" << "\n" << response.getBody();
    return o;
}
