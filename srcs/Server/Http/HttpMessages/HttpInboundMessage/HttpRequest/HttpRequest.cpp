#include "HttpRequest.hpp"
#include "String.hpp"
#include "Number.hpp"
#include "Logger.hpp"
#include "webserv.hpp"

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
#include "httpExceptions.hpp"
#include "Config.hpp"
#include "Location.hpp"

HttpRequest::HttpRequest() :
	AHttpInboundMessage()
{
	reset();
}


HttpRequest &HttpRequest::reset()
{
	m_queryString.clear();
	m_segments.clear();
	m_activeLocations.clear();
	m_rawStr.clear();
	m_target.clear();
	m_headers.clear();
	m_body.clear();
	m_accepts.clear();
	m_connectionKeepAlive = false;
	m_method			  = HttpMethod();
	m_firstLineEndPos	  = 0;
	m_headersEndPos		  = 0;
	m_totalLen			  = 0;
	m_isFirstLineParsed	  = false;
	return *this;
}



/**
 *
 * @brief Retrieves the query string from the request target.
 *
 * @return A string representing the query string.
 *
 */
std::string HttpRequest::getQueryString() const
{
	return m_queryString;
}
/**
 *
 * @brief Retrieves the segments of the request target path.
 *
 * @return A constant reference to a vector of strings representing the path segments.
 *
 */
const Vec<std::string> &HttpRequest::getSegments() const
{
	return m_segments;
}
/**
 *
 * @brief Retrieves the number of segments in the request target path.
 *
 * @return The size of the segments vector.
 *
 */
size_t HttpRequest::getSegmentSize() const
{
	return m_segments.size();
}
/**
 *
 * @brief Retrieves the active locations matched for the request.
 *
 * @return A constant reference to a vector of pointers to the active Config::Location objects.
 *
 */
const Vec<const Config::Location *> &HttpRequest::getActiveLocations() const
{
	return m_activeLocations;
}

/**
 *
 * @brief Retrieves the accepted content types from the request.
 *
 * @return A constant reference to a vector of Accept objects representing the accepted content types.
 *
 */
const Vec<Accept> &HttpRequest::getAccepts() const
{
	return m_accepts;
}

bool HttpRequest::determineConnectionKeepAlive() const
{
	const std::string connectionHeader = getHeader(HEADER_CONNECTION);

	if (connectionHeader == HEADER_CONNECTION_KEEP_ALIVE)
		return true;
	if (connectionHeader == HEADER_CONNECTION_CLOSE)
		return false;
	return false;
}

bool HttpRequest::getConnectionKeepAlive() const
{
	return m_connectionKeepAlive;
}

/**
 *
 * @brief Reads data from the given socket file descriptor
 * and appends it to the raw request string.
 *
 * @param socketFd The socket file descriptor to read from.
 *
 * @return -1 on error, or the number of bytes read.
 *
 */
ssize_t HttpRequest::readOnce(int socketFd)
{
	char		  buffer[READ_BUFFER_SIZE];
	const ssize_t bytesRead = recv(socketFd, buffer, READ_BUFFER_SIZE, MSG_DONTWAIT);

	if (bytesRead < 0)
		return -1;

	m_rawStr.append(buffer, bytesRead);
	return bytesRead;
}



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
	const size_t lineEndSize = std::strlen(HEADER_LINE_ENDING);

	size_t pos = m_firstLineEndPos + lineEndSize;
	AHttpInboundMessage::parse(m_rawStr, pos);

	if (getHeader(HEADER_HOST).empty())
		throw std::invalid_argument("Missing Host Header Field");
	parseActiveLocations(serverConfig);
	parseAccepts(getHeader(HEADER_ACCEPT));
	std::cout << *this << std::endl;
	m_connectionKeepAlive = determineConnectionKeepAlive();
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
		m_target	  = String::urlDecode(path.substr(0, queryPos));
		m_segments	  = String::split(m_target, "/");
		m_queryString = path.substr(queryPos + 1);
		return;
	}
	m_target   = String::urlDecode(path);
	m_segments = String::split(m_target, "/");
}


void HttpRequest::parseAccepts(const std::string &headerValue)
{
	const Vec<std::string> parts = String::split(headerValue, ",");
	for (Vec<std::string>::size_type i = 0; i < parts.size(); ++i)
		m_accepts.push(Accept(parts[i]));
	std::sort(m_accepts.begin(), m_accepts.end(), Accept::compare);
}


/**
 *
 * @brief Parses and stores the matched locations from the server configuration
 * based on the request target segments.
 *
 * @param config The server configuration containing location mappings.
 *
 */
void HttpRequest::parseActiveLocations(const Config &config)
{
	const Map<const Config::Location *> &locs = config.getLocations();

	m_activeLocations.push(config.getDefaultLocation());
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
			m_activeLocations.push(locs.get(currentPath));
		if (i == m_segments.size() - 1)
		{
			if (m_target[m_target.size() - 1] != '/')
				break;
		}
		currentPath += "/";
		if (locs.has(currentPath))
			m_activeLocations.push(locs.get(currentPath));
	}
	std::sort(m_activeLocations.begin(), m_activeLocations.end(), Config::Location::compare);
}



/**
 *
 * @brief Parses the first line of the raw HTTP request string,
 * extracting the HTTP method, target, and version.
 *
 * @return The position of the end of the first line.
 *
 * @throws AHttpException if the request line is invalid or has unsupported HTTP version.
 *
 */
size_t HttpRequest::parseFirstLine()
{
	const size_t lineEnd = m_rawStr.find(HEADER_LINE_ENDING);
	if (lineEnd == std::string::npos)
		throw std::invalid_argument("Invalid Request Line");

	const std::string requestLine = m_rawStr.substr(0, lineEnd);
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
 * @brief Reads the Content-Length header from the HTTP request.
 *
 * @param headers The raw HTTP header string.
 *
 * @throws Http400Exception if the Content-Length header is malformed.
 *
 * @return The value of the Content-Length header, or 0 if not present or invalid.
 *
 */
ssize_t HttpRequest::readContentLength(const std::string &headers)
{
	const size_t len   = headers.size();
	size_t		 clPos = headers.find(HEADER_CONTENT_LENGTH);
	if (clPos == std::string::npos)
		throw Http400Exception();


	clPos += std::strlen(HEADER_CONTENT_LENGTH);
	while (clPos < len && std::isspace(static_cast<unsigned char>(headers[clPos])))
		++clPos;
	if (headers[clPos] != ':')
		throw Http400Exception();


	++clPos;
	while (clPos < len && std::isspace(static_cast<unsigned char>(headers[clPos])))
		++clPos;
	size_t endPos = clPos;
	while (endPos < len && std::isdigit(static_cast<unsigned char>(headers[endPos])))
		++endPos;


	if (endPos <= clPos)
		throw Http400Exception();
	return Number::toSizeT(headers.substr(clPos, endPos - clPos));
}



/**
 *
 * @brief Reads the HTTP method from the raw request string.
 *
 * @param currentRawStr The raw HTTP request string.
 *
 * @return A string representing the HTTP method.
 *
 */
std::string HttpRequest::readMethodString(const std::string &currentRawStr) const
{
	const size_t lineEnd = currentRawStr.find(HEADER_LINE_ENDING);
	if (lineEnd == std::string::npos)
		return "";

	const std::string requestLine = currentRawStr.substr(0, lineEnd);
	const size_t	  methodEnd	  = requestLine.find(' ');
	if (methodEnd != std::string::npos)
		return requestLine.substr(0, methodEnd);
	return "";
}





/**
 *
 * @brief Builds the resource path for the request based on the active locations
 * and the specified segment index.
 *
 * @param config The server configuration.
 * @param segmentIndex The index of the segment to build the resource path for.
 *
 * @return The constructed resource path as a string.
 *
 * @throws std::out_of_range if the segment index is out of range.
 *
 */
std::string HttpRequest::buildResourcePath(const ssize_t segmentIndex) const
{
	if (segmentIndex >= static_cast<ssize_t>(m_segments.size()) || segmentIndex < -1)
		return "";
	const Config::Location *validLocation = NULL;
	bool					isRoot		  = true;

	Vec<const Config::Location *>::const_iterator end = segmentIndex == -1 ? m_activeLocations.begin() + 1 : m_activeLocations.end();
	for (Vec<const Config::Location *>::const_iterator loc = m_activeLocations.begin(); loc != end; ++loc)
	{
		if ((*loc)->hasAlias())
		{
			validLocation = *loc;
			isRoot		  = false;
			continue;
		}
		if ((*loc)->hasRoot())
		{
			validLocation = *loc;
			isRoot		  = true;
		}
	}
	std::string resourcePath = isRoot ? validLocation->getRoot() : validLocation->getAlias();
	if (segmentIndex == -1)
		return resourcePath;
	const size_t startIndex = isRoot ? 0 : String::split(validLocation->getPath(), "/").size();
	const size_t segIndex	= static_cast<size_t>(segmentIndex);
	for (size_t i = startIndex; i <= segIndex; ++i)
	{
		if (resourcePath.at(resourcePath.size() - 1) != '/')
			resourcePath += '/';
		resourcePath += m_segments[i];
	}
	return resourcePath;
}

/**
 *
 * @brief Retrieves the active auto-index setting from the matched locations.
 *
 * @return true if auto-index is enabled, false otherwise.
 *
 */
bool HttpRequest::getActiveAutoIndex() const
{
	Vec<const Config::Location *>::const_iterator it	= m_activeLocations.end();
	Vec<const Config::Location *>::const_iterator begin = m_activeLocations.begin();

	while (it != begin)
	{
		--it;
		if ((*it)->hasAutoIndexSet())
			return (*it)->getAutoIndex();
	}
	return false;
}


/**
 *
 * @brief Retrieves the upload path from the matched locations.
 *
 * @return A string representing the upload path, or an empty string if not set.
 *
 */
std::string HttpRequest::getUploadPath() const
{
	Vec<const Config::Location *>::const_iterator it	= m_activeLocations.end();
	Vec<const Config::Location *>::const_iterator begin = m_activeLocations.begin();

	while (it != begin)
	{
		--it;
		if ((*it)->hasUpload())
		{
			Logger::info("Upload path found: " + (*it)->getUpload());
			return (*it)->getUpload();
		}
	}
	return "";
}

bool HttpRequest::matchAccept(const std::string &contentType) const
{
	for (Vec<Accept>::const_iterator it = m_accepts.begin(); it != m_accepts.end(); ++it)
	{
		const std::string &acceptType = it->getMediaType();
		if (String::match(acceptType, contentType))
			return true;
	}
	return false;
}


/**
 *
 * @brief Checks if any of the active locations allow file uploads.
 *
 * @return true if uploads are allowed, false otherwise.
 *
 */
bool HttpRequest::hasUpload() const
{
	Vec<const Config::Location *>::const_iterator it	= m_activeLocations.end();
	Vec<const Config::Location *>::const_iterator begin = m_activeLocations.begin();

	while (it != begin)
	{
		--it;
		if ((*it)->hasUpload())
			return true;
	}
	return false;
}


/**
 *
 * @brief Checks if the HTTP method of the request is allowed
 * based on the active locations' configurations.
 *
 * @return true if the method is allowed, false otherwise.
 *
 */
bool HttpRequest::isAllowedMethod() const
{
	Vec<const Config::Location *>::const_iterator it	= m_activeLocations.end();
	Vec<const Config::Location *>::const_iterator begin = m_activeLocations.begin();

	while (it != begin)
	{
		--it;
		if ((*it)->hasMethods())
			return (*it)->getAllowedMethods().contains(m_method);
	}
	return true;
}



/**
 *
 * @brief Checks if the HTTP request is complete,
 * meaning it has received all headers and body based on Content-Length.
 *
 * @note Updates m_totalLen member.
 * @note Updates m_isFirstLineParsed member.
 * @note Updates m_firstLineEndPos member.
 *
 * @note It is meant to be called multiple times as data is read from the socket.
 *
 * @throws AHttpException if the request is malformed.
 *
 * @return true if the request is complete, false otherwise.
 *
 */
bool HttpRequest::isComplete()
{
	if (m_totalLen)
		return m_rawStr.size() >= m_totalLen;
	if (!m_isFirstLineParsed)
	{
		size_t headersEnd = m_rawStr.find(HEADER_ENDING);
		if (headersEnd == std::string::npos)
			return false;
		m_headersEndPos		= headersEnd;
		m_firstLineEndPos	= parseFirstLine();
		m_isFirstLineParsed = true;
	}
	const HttpMethod::Type methodType = m_method.getType();
	if (methodType != HttpMethod::POST &&
		methodType != HttpMethod::PUT &&
		methodType != HttpMethod::PATCH)
		return true;
	const ssize_t contentLength = readContentLength(m_rawStr);
	m_totalLen					= m_headersEndPos + std::strlen(HEADER_ENDING) + contentLength;
	return m_rawStr.size() >= m_totalLen;
}


std::ostream &operator<<(std::ostream &o, const HttpRequest &message)
{
	Logger::log("----- HTTP Request -----\n");
	Logger::log(BLUE "Method: " + message.getMethod().getStrType() + RESET);
	Logger::log(GREEN "Target: " + message.getTarget() + RESET);
	Logger::log(CYAN "Query String: " + message.getQueryString() + RESET);
	Logger::log(CYAN "Segments: " + LoggerJson::get(message.getSegments()) + RESET);
	Logger::log(YELLOW "Headers:");
	Logger::log(message.getHeaders());
	Logger::log(YELLOW "Active Locations:").log(LoggerJson::get(message.getActiveLocations()));
	Logger::log(YELLOW "Accepts:").log(LoggerJson::get(message.getAccepts()));
	Logger::log(MAGENTA "Body:" RESET "\n");
	Logger::log(message.getBody());
	Logger::log(RESET "------------------------");
	return o;
}
