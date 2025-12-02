#include "HttpRequest.hpp"
#include "config.hpp"
#include "String.hpp"
#include "Number.hpp"

#include <cctype>
#include <cstring>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <string> 
#include <cstring>
#include <cctype>    
#include <stdexcept> 
#include <iostream>  

// ANCHOR Constructors / Destructors
HttpRequest::HttpRequest(): AHttpMessage() {}
HttpRequest::~HttpRequest() {}


// ANCHOR Getters
std::string HttpRequest::getRawRequest() const
{
    return m_rawStr;
}
std::string HttpRequest::getTarget() const
{
    return m_target;
}

// ANCHOR Setters
void HttpRequest::setRawRequest(const std::string &rawRequest)
{
    m_rawStr = rawRequest;
}
void HttpRequest::setTarget(const std::string &path)
{
    m_target = path;
}


// ANCHOR Reading Public method
HttpRequest& HttpRequest::read(const int clientSocket)
{
    std::string         request = readHeaders(clientSocket);
    const size_t        headersEnd = request.find(HEADER_ENDING);
    
    if (headersEnd == std::string::npos)
        throw std::runtime_error("Incomplete headers received");

    const size_t contentLength = readContentLength(request, headersEnd);
    if (contentLength > 0)
    {
        const size_t bodyStart = headersEnd + std::strlen(HEADER_ENDING);
        const size_t received = request.size() - bodyStart;
        request += readBody(clientSocket, contentLength, received);
    }
    if (!request.empty() && request[request.size() - 1] == '\0')
        request.resize(request.size() - 1);
    m_rawStr = request;
    return *this;
}

// ANCHOR Parsing Public method
HttpRequest& HttpRequest::parse()
{
    const size_t    lineEndSize = std::strlen(HEADER_ENDING);
    std::string     rawRequest = m_rawStr;
    size_t          pos = parseFirstLine(rawRequest) + std::strlen(HEADER_LINE_ENDING);
    size_t          headersEnd = rawRequest.find(HEADER_ENDING, pos);

    if (headersEnd == std::string::npos)
        throw std::invalid_argument("Invalid Headers");
    while (pos < headersEnd)
        pos = parseHeaderLine(rawRequest, pos);
    size_t bodyStart = headersEnd + lineEndSize;
    if (bodyStart < rawRequest.size())
        setBody(rawRequest.substr(bodyStart));

    std::cout << *this << std::endl;
    if (getHeader(HEADER_HOST).empty())
        throw std::invalid_argument("Missing Host Header sadaw");
    
    return *this;
}





// ANCHOR Parsing Private methods
size_t HttpRequest::parseFirstLine(const std::string &rawRequest)
{
    size_t lineEnd = rawRequest.find(HEADER_LINE_ENDING);
    if (lineEnd == std::string::npos)
        throw std::invalid_argument("Invalid Request Line");

    std::string     requestLine = rawRequest.substr(0, lineEnd);
    size_t          methodEnd = requestLine.find(' ');
    if (methodEnd != std::string::npos)
        setMethod(Method(requestLine.substr(0, methodEnd)));

    size_t          targetEnd = requestLine.find(' ', methodEnd + 1);
    if (targetEnd != std::string::npos)
    {
        std::string path = requestLine.substr(methodEnd + 1, targetEnd - methodEnd - 1);
        if (path.empty() || path[0] != '/')
            throw std::invalid_argument("Invalid Request Target: " + path);
        setTarget(path);
    }

    size_t versionPos = requestLine.find(' ', targetEnd + 1);
    if (versionPos != std::string::npos)
    {
        std::string httpVersion = requestLine.substr(versionPos + 1);
        if (httpVersion != PROTOCOL)
            throw std::invalid_argument("Unsupported HTTP Version: " + httpVersion);
    } 
    return lineEnd;
}
size_t  HttpRequest::parseHeaderLine(const std::string &rawRequest, const size_t pos)
{
    const size_t colon = rawRequest.find(':', pos);
    const size_t nextLine = rawRequest.find(HEADER_LINE_ENDING, pos);

    if (colon == std::string::npos || nextLine == std::string::npos || colon > nextLine)
        throw std::invalid_argument("Malformed Header Line");

    const std::string headerName = rawRequest.substr(pos, colon - pos);
    const std::string headerValue = rawRequest.substr(colon + 1, nextLine - colon - 1);
    
    setHeader(headerName, String::trim(headerValue));
    return nextLine + std::strlen(HEADER_LINE_ENDING);
}




// ANCHOR Reading Private method
std::string HttpRequest::readHeaders(const int clientSocket)
{
    std::string     headers;
    ssize_t         bytesRead = 0;
    char            buffer[HEADER_BUFFER_SIZE];

    while (headers.find(HEADER_LINE_ENDING) == std::string::npos)
    {
        bytesRead = recv(clientSocket, buffer, HEADER_BUFFER_SIZE - 1, 0);
        if (bytesRead <= 0)
            break;
        headers.append(buffer, bytesRead);
    }
    if (bytesRead < 0)
        throw std::runtime_error("Receive failed during header line");
    return headers;
}
size_t HttpRequest::readContentLength(std::string &request, const size_t headersEnd)
{
    size_t clPos = request.find(HEADER_CONTENT_LENGTH, 0, headersEnd); 
    if (clPos == std::string::npos)
        return 0;

    clPos += std::strlen(HEADER_CONTENT_LENGTH);
    while (clPos < headersEnd && std::isspace(static_cast<unsigned char>(request[clPos])))
        ++clPos;

    size_t endPos = clPos;
    while (endPos < headersEnd && std::isdigit(static_cast<unsigned char>(request[endPos])))
        ++endPos;

    if (endPos <= clPos)
        return 0;

    std::string clStr = request.substr(clPos, endPos - clPos);
    return Number::toSizeT(clStr);
}
std::string HttpRequest::readBody(
    const int clientSocket,
    const size_t contentLength,
    size_t received
)
{
    std::string     body;
    char            buffer[HEADER_BUFFER_SIZE];
    ssize_t         bytesRead = 0;
    
    while (received < contentLength)
    {
        bytesRead = recv(clientSocket, buffer, HEADER_BUFFER_SIZE - 1, 0);
        if (bytesRead <= 0)
            break;
        body.append(buffer, bytesRead);
        received += bytesRead;
    }
    if (bytesRead < 0)
        throw std::runtime_error("Receive failed during body");
    return body;
}
