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

#include "Config.hpp"
#include "Location.hpp"

HttpRequest::HttpRequest() :
	AHttpInboundMessage(),
	m_queryString("")
{
}
HttpRequest::~HttpRequest()
{
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

	size_t pos = parseFirstLine(m_rawStr) + lineEndSize;
	AHttpInboundMessage::parse(m_rawStr, pos);

	std::cout << *this << std::endl;
	if (getHeader(HEADER_HOST).empty())
		throw std::invalid_argument("Missing Host Header Field");
	parseActiveLocations(serverConfig);
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
void HttpRequest::parseActiveLocations(const Config &config)
{
	Vec<const Config::Location *>		 empty;
	const Map<const Config::Location *> &locs = config.getLocations();

	empty.push(config.getDefaultLocation());
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
	std::sort(empty.begin(), empty.end(), Config::Location::compare);
	m_activeLocations = empty;
}



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
