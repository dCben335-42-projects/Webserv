#include "HttpResponse.hpp"
#include "webserv.hpp"
#include "Number.hpp"
#include <sstream>
#include <sys/socket.h>
#include "httpExceptions.hpp"


HttpResponse::HttpResponse() :
	AHttpMessage()
{
	reset();
}

HttpResponse::~HttpResponse()
{
}



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

	m_headers[HEADER_SERVER] = SERVER_FULL_NAME;
}



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

#include "Logger.hpp"
void HttpResponse::appendToFulFullResponse(const char *buf, size_t size)
{
	Logger::debug("Appending to full response: " + Number::toString(size) + " bytes");
	m_fullResponse.append(buf, size);
}



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
		throw std::runtime_error("send() failed");
	m_sentBytes += n;
	return (m_sentBytes >= m_fullResponse.size());
}

void HttpResponse::build()
{
	m_fullResponse = AHttpMessage::build();
}