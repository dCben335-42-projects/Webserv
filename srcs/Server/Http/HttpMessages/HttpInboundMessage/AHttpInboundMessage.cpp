#include "AHttpInboundMessage.hpp"
#include <cstring>
#include <stdexcept>
#include "String.hpp"


AHttpInboundMessage::AHttpInboundMessage() :
	m_rawStr(""),
	m_target("")
{
}

AHttpInboundMessage::~AHttpInboundMessage()
{
}


/**
 *
 * @brief Retrieves the raw HTTP request string.
 *
 * @return A string representing the raw HTTP request.
 *
 */
std::string AHttpInboundMessage::getRawRequest() const
{
	return m_rawStr;
}

/**
 *
 * @brief Retrieves the request target from the HTTP request.
 *
 * @return A string representing the request target.
 *
 */
std::string AHttpInboundMessage::getTarget() const
{
	return m_target;
}



/**
 *
 * @brief Sets the raw HTTP request string.
 *
 * @param rawRequest The raw HTTP request string to be set.
 *
 */
void AHttpInboundMessage::setRawRequest(const std::string &rawRequest)
{
	m_rawStr = rawRequest;
}


/**
 *
 * @brief Parses a single header line from the raw HTTP request string,
 * extracting the header name and value.
 *
 * @param rawRequest The raw HTTP request string.
 * @param pos The position to start parsing from.
 *
 * @return The position of the end of the parsed header line.
 *
 * @throws std::invalid_argument if the header line is malformed.
 *
 */
size_t AHttpInboundMessage::parseHeaderLine(const std::string &rawRequest, const size_t pos)
{
	const size_t colon	  = rawRequest.find(HEADER_SEPARATOR, pos);
	const size_t nextLine = rawRequest.find(HEADER_LINE_ENDING, pos);

	if (colon == std::string::npos || nextLine == std::string::npos || colon > nextLine)
		throw std::invalid_argument("Malformed Header Line");

	const std::string headerName  = rawRequest.substr(pos, colon - pos);
	const std::string headerValue = rawRequest.substr(colon + HEADER_SEPARATOR_SIZE, nextLine - colon - HEADER_SEPARATOR_SIZE);

	setHeader(String::trim(headerName), String::trim(headerValue));
	return nextLine + std::strlen(HEADER_LINE_ENDING);
}



/**
 *
 * @brief Parses the raw HTTP INBOUND message string into its components:
 * the request line, headers, and body.
 *
 * @return A reference to the current AHttpInboundMessage object.
 *
 * @throws std::invalid_argument if the request is malformed or missing required fields.
 *
 */
AHttpInboundMessage &AHttpInboundMessage::parse(std::string &rawRequest, size_t &pos)
{
	const size_t headerEndSize = std::strlen(HEADER_ENDING);
	const size_t headersEnd	   = rawRequest.find(HEADER_ENDING, pos);

	if (headersEnd == std::string::npos)
		throw std::invalid_argument("Invalid Headers");
	while (pos < headersEnd)
		pos = parseHeaderLine(rawRequest, pos);
	const size_t bodyStart = headersEnd + headerEndSize;
	if (bodyStart < rawRequest.size())
		setBody(rawRequest.substr(bodyStart));
	return *this;
}