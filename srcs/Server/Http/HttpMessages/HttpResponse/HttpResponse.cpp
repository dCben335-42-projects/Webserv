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

/**
 *
 * @brief Resets the HttpResponse object to its initial state,
 * clearing all data and setting default values.
 *
 */
void HttpResponse::reset()
{
	m_statusCode = 0;
	m_sentBytes	 = 0;
	m_fullResponse.clear();
	m_headers.clear();
	m_body.clear();
	m_method = HttpMethod();

	m_headers[HEADER_SERVER] = SERVER_NAME;
}



// ANCHOR Setters
/**
 *
 * @brief Sets the HTTP status code of the response.
 *
 * @param code The HTTP status code to be set.
 *
 */
void HttpResponse::setStatusCode(unsigned short code)
{
	m_statusCode = code;
}
void HttpResponse::setFullResponse(const std::string str)
{
	m_fullResponse = str;
}
void HttpResponse::appendToFulFullResponse(const char *buf, size_t size)
{
	m_fullResponse.append(buf, size);
}



// ANCHOR Getters
/**
 *
 * @brief Gets the HTTP status code of the response.
 *
 * @return The HTTP status code.
 *
 */
unsigned short HttpResponse::getStatusCode() const
{
	return m_statusCode;
}

/**
 *
 * @brief Gets the full HTTP response string.
 *
 * @return The complete HTTP response.
 *
 */
std::string HttpResponse::getFullResponse() const
{
	return m_fullResponse;
}

/**
 *
 * @brief Gets the number of bytes that have been sent so far.
 *
 * @return The number of bytes sent.
 *
 */
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
/**
 *
 * @brief Builds the full HTTP response string,
 * including the status line, headers, and body.
 *
 * @note This function constructs the complete HTTP response
 * and stores it in the m_fullResponse member variable.
 *
 * @note It is typically called after setting the status code,
 * headers, and body of the response.
 *
 */
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


/**
 *
 * @brief Sends the HTTP response over the given socket.
 *
 * @param socket The socket file descriptor to send the response to.
 *
 * @note This function attempts to send a portion of the response,
 *       handling partial sends and non-blocking socket behavior.
 *
 * @return true if the entire response has been sent, false otherwise.
 *
 * @throws std::runtime_error if send() fails.
 *
 */
bool HttpResponse::sendOnce(int socket)
{
	if (m_sentBytes >= m_fullResponse.size())
		return true;
	const char	 *buf		= m_fullResponse.c_str();
	const size_t  remaining = m_fullResponse.size() - m_sentBytes;
	const size_t  to_send	= remaining < WRITE_BUFFER_SIZE ? remaining : WRITE_BUFFER_SIZE;
	const ssize_t n			= send(socket, buf + m_sentBytes, to_send, MSG_NOSIGNAL);

	if (n < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return false;
		return false;
		// throw std::runtime_error("send() failed");
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
