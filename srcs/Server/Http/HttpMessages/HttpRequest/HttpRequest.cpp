#include "HttpRequest.hpp"
#include "config.hpp"
#include "String.hpp"
#include "Number.hpp"
#include "Logger.hpp"

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
#include "File.hpp"

#include "Config.hpp"
#include "Location.hpp"

// ANCHOR Constructors / Destructors
HttpRequest::HttpRequest() :
	AHttpMessage(),
	m_rawStr(""),
	m_target(""),
	m_queryString("")
{
}
HttpRequest::~HttpRequest()
{
}

// ANCHOR Getters
std::string HttpRequest::getRawRequest() const
{
	return m_rawStr;
}
std::string HttpRequest::getTarget() const
{
	return m_target;
}
std::string HttpRequest::getQueryString() const
{
	return m_queryString;
}
Vec<std::string> HttpRequest::getSegments() const
{
	return m_segments;
}

// ANCHOR Setters
void HttpRequest::setRawRequest(const std::string &rawRequest)
{
	m_rawStr = rawRequest;
}


// ANCHOR Socket Reading Public method
/**
 *
 * @brief Reads data from the given socket file descriptor
 * and appends it to the raw request string.
 *
 * @param socketFd The socket file descriptor to read from.
 *
 * @return The number of bytes read, or 0 if the connection is closed.
 *
 * @throws std::runtime_error if recv() fails.
 *
 */
ssize_t HttpRequest::readOnce(int socketFd)
{
	char		  buffer[READ_BUFFER_SIZE];
	const ssize_t bytesRead = recv(socketFd, buffer, READ_BUFFER_SIZE, 0);

	if (bytesRead < 0)
		throw std::runtime_error("recv() failed");
	if (bytesRead == 0)
		return 0;

	m_rawStr.append(buffer, bytesRead);
	return bytesRead;
}


// ANCHOR Parsing Public method

/**
 *
 * @brief Parses the raw HTTP request string into its components:
 * the request line, headers, and body.
 *
 * @return A reference to the current HttpRequest object.
 *
 * @throws std::invalid_argument if the request is malformed or missing required fields.
 *
 */
HttpRequest &HttpRequest::parse(const Config &serverConfig)
{
	const size_t headerEndSize = std::strlen(HEADER_ENDING);
	const size_t lineEndSize   = std::strlen(HEADER_LINE_ENDING);

	std::string rawRequest = m_rawStr;
	size_t		pos		   = parseFirstLine(rawRequest) + lineEndSize;
	size_t		headersEnd = rawRequest.find(HEADER_ENDING, pos);

	if (headersEnd == std::string::npos)
		throw std::invalid_argument("Invalid Headers");
	while (pos < headersEnd)
		pos = parseHeaderLine(rawRequest, pos);
	size_t bodyStart = headersEnd + headerEndSize;
	if (bodyStart < rawRequest.size())
		setBody(rawRequest.substr(bodyStart));

	std::cout << *this << std::endl;
	if (getHeader(HEADER_HOST).empty())
		throw std::invalid_argument("Missing Host Header Field");
	parseMatchedLocations(serverConfig);
	return *this;
}


/**
 *
 * @brief Parses the target path to separate the query string from the target.
 *
 * @param path The full request target path.
 *
 * @note Updates m_target and m_queryString members.
 *
 */
void HttpRequest::parseTargetQuery(const std::string &path)
{
	const size_t queryPos = path.find('?');

	if (queryPos != std::string::npos)
	{
		m_target	  = path.substr(0, queryPos);
		m_segments	  = String::split(m_target, "/");
		m_queryString = path.substr(queryPos + 1);
		return;
	}
	m_segments = String::split(path, "/");
	m_target   = path;
}

/**
 *
 * @brief Parses and stores the matched locations from the server configuration
 * based on the request target segments.
 *
 * @param config The server configuration containing location mappings.
 *
 */
void HttpRequest::parseMatchedLocations(const Config &config)
{
	Vec<const Config::Location *>		 empty;
	const Map<const Config::Location *> &locs = config.getLocations();

	for (Vec<std::string>::size_type i = 0; i < m_segments.size(); ++i)
	{
		std::string currentPath = "/";
		for (Vec<std::string>::size_type j = 0; j <= i; ++j)
		{
			if (j > 0)
				currentPath += "/";
			currentPath += m_segments[j];
		}
		if (locs.has(currentPath))
			empty.push(locs.get(currentPath));
		if (i == m_segments.size() - 1)
		{
			if (m_target[m_target.size() - 1] != '/')
				break;
		}
		currentPath += "/";
		if (locs.has(currentPath))
			empty.push(locs.get(currentPath));
	}

	m_activeLocations = empty;
}


// ANCHOR Parsing Private methods

/**
 *
 * @brief Parses the first line of the raw HTTP request string,
 * extracting the HTTP method, target, and version.
 *
 * @param rawRequest The raw HTTP request string.
 *
 * @return The position of the end of the first line.
 *
 * @throws std::invalid_argument if the request line is malformed.
 *
 */
size_t HttpRequest::parseFirstLine(const std::string &rawRequest)
{
	const size_t lineEnd = rawRequest.find(HEADER_LINE_ENDING);
	if (lineEnd == std::string::npos)
		throw std::invalid_argument("Invalid Request Line");

	const std::string requestLine = rawRequest.substr(0, lineEnd);
	const size_t	  methodEnd	  = requestLine.find(' ');
	if (methodEnd != std::string::npos)
		setMethod(HttpMethod(requestLine.substr(0, methodEnd)));

	const size_t targetEnd = requestLine.find(' ', methodEnd + 1);
	if (targetEnd != std::string::npos)
	{
		std::string path = requestLine.substr(methodEnd + 1, targetEnd - methodEnd - 1);
		if (path.empty() || path[0] != '/')
			throw std::invalid_argument("Invalid Request Target: " + path);
		parseTargetQuery(path);
	}

	const size_t versionPos = requestLine.find(' ', targetEnd + 1);
	if (versionPos != std::string::npos)
	{
		const std::string httpVersion = requestLine.substr(versionPos + 1);
		if (httpVersion != PROTOCOL)
			throw std::invalid_argument("Unsupported HTTP Version: " + httpVersion);
	}
	return lineEnd;
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
size_t HttpRequest::parseHeaderLine(const std::string &rawRequest, const size_t pos)
{
	const size_t colon	  = rawRequest.find(':', pos);
	const size_t nextLine = rawRequest.find(HEADER_LINE_ENDING, pos);

	if (colon == std::string::npos || nextLine == std::string::npos || colon > nextLine)
		throw std::invalid_argument("Malformed Header Line");

	const std::string headerName  = rawRequest.substr(pos, colon - pos);
	const std::string headerValue = rawRequest.substr(colon + 1, nextLine - colon - 1);

	setHeader(headerName, String::trim(headerValue));
	return nextLine + std::strlen(HEADER_LINE_ENDING);
}


/**
 *
 * @brief Reads the Content-Length header from the HTTP request.
 *
 * @param request The raw HTTP request string.
 * @param headersEnd The position of the end of the headers in the request string.
 *
 * @return The value of the Content-Length header, or 0 if not present or invalid.
 *
 */
size_t HttpRequest::readContentLength(std::string request, const size_t headersEnd)
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

	return Number::toSizeT(request.substr(clPos, endPos - clPos));
}



const Vec<const Config::Location *> &HttpRequest::getActiveLocations() const
{
	return m_activeLocations;
}



// ANCHOR Overloaded Operators
std::ostream &operator<<(std::ostream &o, const HttpRequest &message)
{
	Logger::log("----- HTTP Request -----\n");
	Logger::log(BLUE "Method: " + message.getMethod() + RESET);
	Logger::log(GREEN "Target: " + message.getTarget() + RESET);
	Logger::log(CYAN "Query String: " + message.getQueryString() + RESET);
	Logger::log(CYAN "Segments: " + LoggerJson::get(message.getSegments()) + RESET);
	Logger::log(YELLOW "Headers:");
	Logger::log(message.getHeaders());
	Logger::log(MAGENTA "Body:" RESET "\n");
	Logger::log(message.getBody());
	Logger::log(RESET "------------------------");
	return o;
}
