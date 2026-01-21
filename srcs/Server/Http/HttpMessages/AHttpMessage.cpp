#include "AHttpMessage.hpp"
#include "Number.hpp"
#include "config.hpp"
#include <exception>
#include "Logger.hpp"

AHttpMessage::AHttpMessage() :
	m_method(),
	m_body("")
{
}

AHttpMessage::~AHttpMessage()
{
}



// ANCHOR Getters
/**
 *
 * @brief Gets all HTTP headers of the message.
 *
 * @return A map containing all HTTP headers.
 *
 */
Map<std::string> AHttpMessage::getHeaders() const
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
	return m_headers.get(key);
}



/**
 *
 * @brief Gets the HTTP method of the message.
 *
 * @return The HTTP method as a string.
 *
 */
std::string AHttpMessage::getMethod() const
{
	return m_method.getStrType();
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




// ANCHOR Setters
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





// ANCHOR Overload operator <<
std::ostream &operator<<(std::ostream &o, AHttpMessage const &message)
{
	Logger::log(message.getHeaders());
	return o;
}
