#include "AHttpMessage.hpp"
#include "Number.hpp"
#include "Logger.hpp"
#include "webserv.hpp"
#include "httpExceptions.hpp"

Map<unsigned short, std::string> AHttpMessage::initStatusReasonMap()
{
	Map<unsigned short, std::string> map;

	map[200] = HTTP_200_MSG;
	map[201] = HTTP_201_MSG;
	map[204] = HTTP_204_MSG;
	map[400] = HTTP_400_MSG;
	map[401] = HTTP_401_MSG;
	map[403] = HTTP_403_MSG;
	map[404] = HTTP_404_MSG;
	map[405] = HTTP_405_MSG;
	map[500] = HTTP_500_MSG;
	map[501] = HTTP_501_MSG;
	return (map);
}

const Map<unsigned short, std::string> AHttpMessage::m_statusReasonMap =
	AHttpMessage::initStatusReasonMap();



AHttpMessage::AHttpMessage() :
	m_method(),
	m_body(""),
	m_statusCode(0)
{
}

AHttpMessage::~AHttpMessage()
{
}



/**
 *
 * @brief Gets all HTTP headers of the message.
 *
 * @return A map containing all HTTP headers.
 *
 */
const Map<std::string> &AHttpMessage::getHeaders() const
{
	return m_headers;
}


/**
 *
 * @brief Gets the value of a specific HTTP header.
 *
 * @param key The key of the header to retrieve.
 *
 * @return The value of the specified header.
 *
 */
std::string AHttpMessage::getHeader(const std::string &key) const
{
	std::string value;
	return m_headers.try_get(key, value) ? value : "";
}



/**
 *
 * @brief Gets the HTTP method of the message.
 *
 * @return The HTTP method as a string.
 *
 */
const HttpMethod &AHttpMessage::getMethod() const
{
	return m_method;
}
/**
 *
 * @brief Gets the body of the HTTP message.
 *
 * @return The body of the HTTP message.
 *
 */
std::string AHttpMessage::getBody() const
{
	return m_body;
}




/**
 *
 * @brief Sets a specific HTTP header.
 *
 * @param key The key of the header to set.
 * @param value The value of the header to set.
 *
 */
void AHttpMessage::setHeader(const std::string &key, const std::string &value)
{
	m_headers.set(key, value);
}
/**
 *
 * @brief Sets the body of the HTTP message.
 *
 * @param body The body content to be set.
 *
 */
void AHttpMessage::setBody(const std::string &body)
{
	m_body							 = body;
	m_headers[HEADER_CONTENT_LENGTH] = Number::toString(body.size());
}

/**
 *
 * @brief Appends data to the body of the HTTP message.
 *
 * @param data The data to be appended to the body.
 *
 */
void AHttpMessage::appendBody(const std::string &data)
{
	m_body += data;
	m_headers[HEADER_CONTENT_LENGTH] = Number::toString(m_body.size());
}

/**
 *
 * @brief Appends data to the body of the HTTP message.
 *
 * @param buffer The buffer containing data to be appended.
 * @param size The size of the data to be appended.
 *
 */
void AHttpMessage::appendBody(const char *buffer, size_t size)
{
	m_body.append(buffer, size);
	m_headers[HEADER_CONTENT_LENGTH] = Number::toString(m_body.size());
}



/**
 *
 * @brief Sets the HTTP method of the message.
 *
 * @param method The HTTP method to be set.
 *
 */
void AHttpMessage::setMethod(const HttpMethod &method)
{
	m_method = method;
}




std::ostream &operator<<(std::ostream &o, AHttpMessage const &message)
{
	Logger::log(message.getHeaders());
	return o;
}

/**
 *
 * @brief Builds the status line of the HTTP response.
 *
 * @return A string representing the HTTP status line.
 *
 */
std::string AHttpMessage::buildHead() const
{
	const Map<unsigned short, std::string>::const_iterator it =
		m_statusReasonMap.find(m_statusCode);
	std::ostringstream head;

	head << PROTOCOL << " " << m_statusCode << " ";
	head << (it != m_statusReasonMap.end() ? it->second : "Unknown Status") << HEADER_LINE_ENDING;
	return head.str();
}


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
std::string AHttpMessage::build()
{
	std::ostringstream responseStream;
	responseStream << buildHead();
	Map<std::string>::const_iterator it = m_headers.begin();
	for (; it != m_headers.end(); ++it)
		responseStream << it->first << HEADER_SEPARATOR << it->second << HEADER_LINE_ENDING;
	responseStream << HEADER_LINE_ENDING;
	responseStream << m_body;
	return responseStream.str();
}