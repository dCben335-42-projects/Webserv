#include "HttpResponse.hpp"
#include <sstream>
#include <sys/socket.h>
#include "config.hpp"
#include <iostream>
#include "Number.hpp"
#include <cerrno>

Map<unsigned short, std::string> HttpResponse::initStatusReasonMap()
{
	Map<unsigned short, std::string> map;

	map[200] = "OK";
	map[201] = "Created";
	map[204] = "No Content";
	map[400] = "Bad Request";
	map[401] = "Unauthorized";
	map[403] = "Forbidden";
	map[404] = "Not Found";
	map[405] = "Method Not Allowed";
	map[413] = "Payload Too Large";
	map[414] = "URI Too Long";
	map[500] = "Internal Server Error";
	map[501] = "Not Implemented";
	return (map);
}

const Map<unsigned short, std::string> HttpResponse::m_statusReasonMap =
	HttpResponse::initStatusReasonMap();


HttpResponse::HttpResponse()
{
	reset();
}

HttpResponse::~HttpResponse()
{
}


// ANCHOR Reset
void HttpResponse::reset()
{
	m_statusCode = 0;
	m_sentBytes	 = 0;
	m_fullResponse.clear();
	m_headers.clear();
	m_body.clear();
	m_method = HttpMethod();
}



// ANCHOR Setters
void HttpResponse::setStatusCode(unsigned short code)
{
	m_statusCode = code;
}

// ANCHOR Getters
unsigned short HttpResponse::getStatusCode() const
{
	return m_statusCode;
}

std::string HttpResponse::getFullResponse() const
{
	return m_fullResponse;
}

size_t HttpResponse::getSentBytes() const
{
	return m_sentBytes;
}

// ANCHOR Build head
std::string HttpResponse::buildHead() const
{
	const Map<unsigned short, std::string>::const_iterator it =
		m_statusReasonMap.find(m_statusCode);
	std::ostringstream head;

	head << PROTOCOL << " " << m_statusCode << " ";
	head << (it != m_statusReasonMap.end() ? it->second : "Unknown Status") << HEADER_LINE_ENDING;
	return head.str();
}


// ANCHOR Send response
void HttpResponse::build()
{
	std::ostringstream responseStream;
	responseStream << buildHead();
	Map<std::string>::const_iterator it = m_headers.begin();
	for (; it != m_headers.end(); ++it)
		responseStream << it->first << ": " << it->second << HEADER_LINE_ENDING;
	responseStream << HEADER_LINE_ENDING;
	responseStream << m_body;
	m_fullResponse = responseStream.str();
}

bool HttpResponse::sendRes(int socket)
{
	if (m_sentBytes >= m_fullResponse.size())
		return true;
	const char	 *buf		= m_fullResponse.c_str();
	const size_t  remaining = m_fullResponse.size() - m_sentBytes;
	const size_t  to_send	= remaining < WRITE_BUFFER_SIZE ? remaining : WRITE_BUFFER_SIZE;
	const ssize_t n			= send(socket, buf + m_sentBytes, to_send, 0);

	if (n < 0)
	{
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
	o << response.buildHead() << static_cast<AHttpMessage const &>(response);
	o << "\n"
	  << "Body:" << "\n"
	  << response.getBody();
	return o;
}
