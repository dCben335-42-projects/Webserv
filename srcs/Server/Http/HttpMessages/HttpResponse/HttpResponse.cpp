#include "HttpResponse.hpp"
#include "webserv.hpp"
#include "Number.hpp"
#include <sstream>
#include <sys/socket.h>
#include "httpExceptions.hpp"
#include "Date.hpp"

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
 * This function is useful for reusing the HttpResponse object for multiple requests,
 * avoiding the need to create a new instance each time.
 *
 * @return A reference to the current HttpResponse object after resetting.
 *
 */
HttpResponse &HttpResponse::reset()
{
	m_statusCode = 0;
	m_sentBytes	 = 0;
	m_fullResponse.clear();
	m_headers.clear();
	m_body.clear();
	m_method				 = HttpMethod();
	m_headers[HEADER_SERVER] = SERVER_FULL_NAME;
	return *this;
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
void HttpResponse::appendToFullResponse(const char *buf, size_t size)
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
	const size_t totalSize = m_fullResponse.size();
	if (m_sentBytes >= totalSize)
		return true;
	const char	 *buf		= m_fullResponse.c_str();
	const size_t  remaining = totalSize - m_sentBytes;
	const size_t  to_send	= remaining < WRITE_BUFFER_SIZE ? remaining : WRITE_BUFFER_SIZE;
	const ssize_t n			= send(socket, buf + m_sentBytes, to_send, MSG_NOSIGNAL);

	if (n < 0)
		throw std::runtime_error("Failed to send response: " + std::string(strerror(errno)));
	m_sentBytes += n;
	return (m_sentBytes >= totalSize);
}


/**
 *
 * @brief Builds the full HTTP response string,
 * including the status line, headers, and body.
 *
 * @note This function constructs the complete HTTP response
 *       and stores it in the m_fullResponse member variable.
 *
 * @return A reference to the current HttpResponse object after building the response.
 *
 */
HttpResponse &HttpResponse::build()
{
	m_fullResponse = AHttpMessage::build();
	return *this;
}


HttpResponse &HttpResponse::fill(bool shouldClose, const time_t keepAliveTimeout)
{
	m_headers[HEADER_CONTENT_LENGTH] = Number::toString(m_body.size());
	m_headers[HEADER_DATE]			 = Date::getCurrentHttpDate();

	bool isCLoseStatus = AHttpMessage::isCloseStatus(m_statusCode);
	if (shouldClose || isCLoseStatus)
		m_headers[HEADER_CONNECTION] = HEADER_CONNECTION_CLOSE;
	else
	{
		m_headers[HEADER_CONNECTION] = HEADER_CONNECTION_KEEP_ALIVE;
		m_headers[HEADER_KEEP_ALIVE] = "timeout=" + Number::toString(keepAliveTimeout);
	}
	return *this;
}