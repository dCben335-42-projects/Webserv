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

	map[300] = HTTP_300_MSG;
	map[301] = HTTP_301_MSG;

	map[400] = HTTP_400_MSG;
	map[401] = HTTP_401_MSG;
	map[403] = HTTP_403_MSG;
	map[404] = HTTP_404_MSG;
	map[405] = HTTP_405_MSG;
	map[406] = HTTP_406_MSG;
	map[408] = HTTP_408_MSG;
	map[409] = HTTP_409_MSG;
	map[413] = HTTP_413_MSG;

	map[500] = HTTP_500_MSG;
	map[501] = HTTP_501_MSG;
	map[504] = HTTP_504_MSG;
	return (map);
}

Vec<unsigned short> AHttpMessage::initCloseStatus()
{
	Vec<unsigned short> vec;

	vec.push(400);
	vec.push(408);
	vec.push(413);
	return (vec);
}

Vec<unsigned short> AHttpMessage::initRedirectionStatus()
{
	Vec<unsigned short> vec;

	vec.push(300);
	vec.push(301);
	vec.push(302);
	vec.push(303);
	vec.push(307);
	vec.push(308);
	return (vec);
}

const Map<unsigned short, std::string> AHttpMessage::m_statusReasonMap =
	AHttpMessage::initStatusReasonMap();

const Vec<unsigned short> AHttpMessage::m_closeStatus =
	AHttpMessage::initCloseStatus();

const Vec<unsigned short> AHttpMessage::m_redirectionStatus =
	AHttpMessage::initRedirectionStatus();


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
 * @return A reference to a map containing all HTTP headers.
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
 * @return The value of the specified header or an empty string
 * if the header is not present in the message.
 *
 */
std::string AHttpMessage::getHeader(const std::string &key) const
{
	std::string lowerKey = String::toLower(key);
	if (m_headers.has(lowerKey))
		return m_headers.get(lowerKey);
	return "";
}


unsigned short AHttpMessage::getStatusCode() const
{
	return m_statusCode;
}

/**
 *
 * @brief Gets the HTTP method of the message.
 *
 * @return A reference to the HTTP method of the message.
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
const std::string &AHttpMessage::getBody() const
{
	return m_body;
}



/**
 *
 * @brief Checks if a specific HTTP header is present in the message.
 *
 * @param key The key of the header to check.
 *
 * @return true if the header is present, false otherwise.
 *
 */
bool AHttpMessage::hasHeader(const std::string &key) const
{
	return m_headers.has(String::toLower(key));
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
	m_headers.set(String::toLower(key), value);
}


/**
 *
 * @brief Deletes a specific HTTP header from the message.
 *
 * @param key The key of the header to delete.
 *
 */
void AHttpMessage::deleteHeader(const std::string &key)
{
	m_headers.remove(String::toLower(key));
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
	m_body = body;
	setHeader(HEADER_CONTENT_LENGTH, Number::toString(body.size()));
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


/**
 *
 * @brief Checks if the current HTTP status code is one that should result in a connection close.
 *
 * @return true if the status code is in the list of close status codes, false otherwise.
 *
 */
bool AHttpMessage::isCloseStatus(unsigned short statusCode) const
{
	return m_closeStatus.contains(statusCode);
}

/**
 *
 * @brief Checks if the current HTTP status code is one that indicates a redirection.
 *
 * @return true if the status code is in the list of redirection status codes, false otherwise.
 *
 */
bool AHttpMessage::isRedirectionStatus(unsigned short statusCode) const
{
	return m_redirectionStatus.contains(statusCode);
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




std::ostream &operator<<(std::ostream &o, AHttpMessage const &message)
{
	o << Logger::get(message.getHeaders());
	return o;
}