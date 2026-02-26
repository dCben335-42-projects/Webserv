#include "HttpRequest.hpp"
#include "String.hpp"
#include "Number.hpp"
#include "webserv.hpp"
#include <sys/socket.h>
#include "httpExceptions.hpp"
#include "Logger.hpp"



HttpRequest::HttpRequest() :
	AHttpInboundMessage()
{
	reset();
}


/**
 *
 * @brief Resets the HttpRequest object to its initial state,
 * clearing all stored data and resetting member variables.
 *
 * @return A reference to the current HttpRequest object.
 *
 */
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
	m_host				  = std::make_pair(false, std::string());
	return *this;
}


/**
 *
 * @brief Retrieves the query string from the request target.
 *
 * @return A string representing the query string.
 *
 */
const std::string &HttpRequest::getQueryString() const
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
 * @return A constant reference to a vector of pointers to the active Location objects.
 *
 */
const Config::Locations &HttpRequest::getActiveLocations() const
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

/**
 *
 * @brief Retrieves the total length of the raw HTTP request string.
 *
 * @return The total length of the raw HTTP request string.
 *
 */
size_t HttpRequest::getTotalLength() const
{
	return m_totalLen;
}



/**
 *
 * @brief Determines whether the connection should be kept alive based on the "Connection" header.
 *
 * @note It is meant to be used internally during request parsing to set the m_connectionKeepAlive member variable.
 *
 * @return true if the connection should be kept alive, false otherwise.
 *
 */
bool HttpRequest::determineConnectionKeepAlive() const
{
	const std::string connectionHeader = getHeader(HEADER_CONNECTION);

	if (connectionHeader == HEADER_CONNECTION_KEEP_ALIVE)
		return true;
	if (connectionHeader == HEADER_CONNECTION_CLOSE)
		return false;
	return false;
}


/**
 *
 * @brief Retrieves whether the connection should be kept alive based on the request headers.
 *
 * @return true if the connection should be kept alive, false otherwise.
 *
 */
bool HttpRequest::getConnectionKeepAlive() const
{
	return m_connectionKeepAlive;
}




/**
 *
 * @brief Checks if a cookie with the specified key exists in the request.
 *
 * @param key The key of the cookie to check for.
 *
 * @return true if the cookie exists, false otherwise.
 *
 */
bool HttpRequest::hasCookieValue(const std::string &key) const
{
	return m_cookieValues.has(key);
}

/**
 *
 * @brief Retrieves the value of a cookie with the specified key from the request.
 *
 * @param key The key of the cookie to retrieve.
 *
 * @return The value of the cookie if it exists, or an empty string if it does not exist.
 *
 */
std::string HttpRequest::getCookieValue(const std::string &key) const
{
	if (!m_cookieValues.has(key))
		return "";
	return m_cookieValues.get(key);
}




/**
 *
 * @brief Parses the raw HTTP request string into its components:
 * the request line, headers, and body.
 *
 * @return A reference to the current HttpRequest object.
 *
 * @throws Http400Exception if the request is malformed or has invalid headers.
 *
 */
HttpRequest &HttpRequest::parse(const Config &serverConfig)
{
	try
	{
		const size_t lineEndSize = std::strlen(HEADER_LINE_ENDING);

		size_t pos = m_firstLineEndPos + lineEndSize;
		AHttpInboundMessage::parse(m_rawStr, pos);

		if (!hasHeader(HEADER_HOST))
			throw std::invalid_argument(REQ_MISSING_HOST_MSG);
		parseActiveLocations(serverConfig);
		parseAccepts(getHeader(HEADER_ACCEPT));
		if (hasHeader(HEADER_CONTENT_LENGTH))
		{
			const size_t contentLength = readContentLength(m_rawStr);
			if (contentLength != m_body.size())
				throw std::invalid_argument(REQ_CONTENT_LENGTH_MISMATCH_MSG);
		}
		std::cout << *this << std::endl;
		m_connectionKeepAlive = determineConnectionKeepAlive();
		parseCookie();
	}
	catch (const std::invalid_argument &e)
	{
		throw Http400Exception(e.what());
	}
	return *this;
}


/**
 *
 * @brief Parses the "Cookie" header from the request and stores the cookie key-value pairs.
 *
 * @return A reference to the current HttpRequest object.
 *
 * @throws std::invalid_argument if the "Cookie" header is malformed.
 *
 */
HttpRequest &HttpRequest::parseCookie()
{
	if (!hasHeader(HEADER_COOKIE))
		return *this;
	const Vec<std::string> keyValues = String::splitPattern(getHeader(HEADER_COOKIE), COOKIE_FIELD_SEP);
	for (Vec<std::string>::const_iterator it = keyValues.begin(); it != keyValues.end(); it++)
	{
		const size_t equalIndex = it->find(COOKIE_VALUE_SEP);
		if (equalIndex == std::string::npos)
			throw std::invalid_argument("Malformed Cookie");
		m_cookieValues.set(it->substr(0, equalIndex), it->substr(equalIndex + 1));
	}
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
		m_target	  = String::urlDecode(String::normalizePath(path.substr(0, queryPos)));
		m_segments	  = String::split(m_target, "/");
		m_queryString = path.substr(queryPos + 1);
		return;
	}
	m_target   = String::urlDecode(String::normalizePath(path));
	m_segments = String::split(m_target, "/");
}


/**
 *
 * @brief Parses the "Accept" header value and stores the accepted content types.
 *
 * @param headerValue The value of the "Accept" header to parse.
 *
 * @note Updates the m_accepts member variable with the parsed Accept objects.
 *
 */
void HttpRequest::parseAccepts(const std::string &headerValue)
{
	const Vec<std::string> &parts = String::split(headerValue, ",");
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
	const Config::LocationsMap &locs = config.getLocations();
	Config::Locations			locations;
	m_activeLocations.push(config.getDefaultLocation());

	if (locs.has("/"))
		locations.push(locs.get("/"));
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
			locations.push(locs.get(currentPath));
		if (i == m_segments.size() - 1)
		{
			if (m_target[m_target.size() - 1] != '/')
				break;
		}
		currentPath += "/";
		if (locs.has(currentPath))
			locations.push(locs.get(currentPath));
	}
	if (FLAG_LOCATION_ADD_UP)
		m_activeLocations += locations;
	else if (locations.size() > 0)
		m_activeLocations.push(locations.back());
	std::sort(m_activeLocations.begin(), m_activeLocations.end(), Location::compare);
}




/**
 *
 * @brief Parses the first line of the raw HTTP request string,
 * extracting the HTTP method, target, and version.
 *
 * @return The position of the end of the first line.
 *
 * @throws std::invalid_argument if the request line is malformed or contains an unsupported HTTP version.
 * @throws Http501Exception if the HTTP method is not supported.
 *
 */
size_t HttpRequest::parseFirstLine(const size_t lineEnd)
{
	const std::string requestLine = m_rawStr.substr(0, lineEnd);
	const size_t	  methodEnd	  = requestLine.find(' ');
	if (methodEnd == std::string::npos)
		throw std::invalid_argument(REQ_INVALID_FIRST_LINE_MSG + requestLine);
	setMethod(HttpMethod(requestLine.substr(0, methodEnd)));

	const size_t targetEnd = requestLine.find(' ', methodEnd + 1);
	if (targetEnd == std::string::npos)
		throw std::invalid_argument(REQ_INVALID_FIRST_LINE_MSG + requestLine);
	std::string path = requestLine.substr(methodEnd + 1, targetEnd - methodEnd - 1);
	if (path.empty() || path[0] != '/')
		throw std::invalid_argument(REQ_INVALID_TARGET_MSG + path);
	parseTargetQuery(path);

	const size_t versionPos = requestLine.find(PROTOCOL, targetEnd + 1);
	if (versionPos == std::string::npos)
		throw std::invalid_argument(REQ_UNSUPPORTED_METHOD_MSG + requestLine.substr(targetEnd + 1));
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
size_t HttpRequest::readContentLength(const std::string &headers) const
{
	const char *toFind = HEADER_LINE_ENDING HEADER_CONTENT_LENGTH;

	const size_t len   = headers.size();
	size_t		 clPos = String::findIC(headers, toFind);
	if (clPos == std::string::npos)
		throw Http400Exception(REQ_MISSING_CONTENT_LENGTH_MSG);
	clPos += std::strlen(toFind);
	if (headers[clPos] != ':')
		throw Http400Exception(REQ_EXPECTED_HEADER_COLON_MSG);
	++clPos;
	while (clPos < len && std::isspace(static_cast<unsigned char>(headers[clPos])))
		++clPos;
	size_t endPos = clPos;
	while (endPos < len && std::isdigit(static_cast<unsigned char>(headers[endPos])))
		++endPos;

	if (endPos <= clPos)
		throw Http400Exception(REQ_INVALID_CONTENT_LENGTH_MSG);
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
 * @brief Reads the Host header value from the raw HTTP request string.
 *
 * @param currentHeader The raw HTTP header string.
 *
 * @return A string representing the Host header value, or an empty string if not found.
 *
 */
std::string HttpRequest::readHost(const std::string &currentHeader) const
{
	const char *toFind = HEADER_LINE_ENDING HEADER_HOST;

	size_t hostPos = String::findIC(currentHeader, toFind);
	if (hostPos == std::string::npos)
		return "";
	const size_t endPos = currentHeader.find(HEADER_LINE_ENDING, hostPos + std::strlen(toFind));
	if (endPos == std::string::npos)
		return "";
	hostPos += std::strlen(toFind);
	if (currentHeader[hostPos] != ':')
		throw Http400Exception(REQ_EXPECTED_HEADER_COLON_MSG);
	++hostPos;
	while (hostPos < endPos && std::isspace(static_cast<unsigned char>(currentHeader[hostPos])))
		++hostPos;
	return currentHeader.substr(hostPos, endPos - hostPos);
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
	const Location *validLocation = NULL;
	bool			isRoot		  = true;

	m_locCIt end = segmentIndex == -1 ? m_activeLocations.begin() + 1 : m_activeLocations.end();
	for (m_locCIt loc = m_activeLocations.begin(); loc != end; ++loc)
	{
		if ((*loc).hasAlias())
		{
			validLocation = &(*loc);
			isRoot		  = false;
			continue;
		}
		if ((*loc).hasRoot())
		{
			validLocation = &(*loc);
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
	m_locCIt it	   = m_activeLocations.end();
	m_locCIt begin = m_activeLocations.begin();

	while (it != begin)
	{
		--it;
		if ((*it).hasAutoIndex())
			return (*it).getAutoIndex();
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
	m_locCIt it	   = m_activeLocations.end();
	m_locCIt begin = m_activeLocations.begin();

	while (it != begin)
	{
		--it;
		if ((*it).hasUpload())
			return (*it).getUpload();
	}
	return "";
}



/**
 *
 * @brief Checks if any of the active locations have a return directive.
 *
 * @return true if a return directive is found, false otherwise.
 *
 */
bool HttpRequest::matchAccept(const std::string &contentType) const
{
	if (m_accepts.empty())
		return true;
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
	m_locCIt it	   = m_activeLocations.end();
	m_locCIt begin = m_activeLocations.begin();

	while (it != begin)
	{
		--it;
		if ((*it).hasUpload())
			return true;
	}
	return false;
}


/**
 *
 * @brief Checks if any of the active locations have a return directive.
 *
 * @return true if a return directive is found, false otherwise.
 *
 */
bool HttpRequest::hasReturn() const
{
	m_locCIt root = m_activeLocations.begin();

	if ((*root).hasReturn())
		return true;
	m_locCIt last = m_activeLocations.end() - 1;
	return (*last).hasReturn();
}


/**
 *
 * @brief Retrieves the return directive from the matched locations.
 *
 * @return A LocationReturn object representing the return directive, or a default-constructed object if not set.
 *
 */
const LocationReturn &HttpRequest::getReturn() const
{
	m_locCIt root = m_activeLocations.begin();

	if ((*root).hasReturn())
		return (*root).getReturn();
	m_locCIt last = m_activeLocations.end() - 1;
	return (*last).getReturn();
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
	const Set<HttpMethod> *allowedMethods = getAllowedMethods();
	if (allowedMethods == NULL)
		return true;
	return allowedMethods->contains(m_method);
}


/**
 *
 * @brief Retrieves the set of allowed HTTP methods based on the active locations' configurations.
 *
 * @return A pointer to a set of HttpMethod objects representing the allowed methods, or nullptr if no methods are set.
 *
 */
const Set<HttpMethod> *HttpRequest::getAllowedMethods() const
{
	m_locCIt it	   = m_activeLocations.end();
	m_locCIt begin = m_activeLocations.begin();

	while (it != begin)
	{
		--it;
		if ((*it).hasMethods())
			return &(*it).getAllowedMethods();
	}
	return NULL;
}

/**
 *
 * @brief Checks if the Host header is present in the request.
 *
 * @return true if the Host header is present, false otherwise.
 *
 */
bool HttpRequest::hasHost() const
{
	return m_host.first;
}

/**
 *
 * @brief Retrieves the value of the Host header from the request.
 *
 * @return A string representing the Host header value.
 *
 */
const std::string &HttpRequest::getHost() const
{
	return m_host.second;
}



/**
 *
 * @brief Checks if the HTTP request headers are complete by looking for the end of headers sequence.
 *
 * @param maxHeadersSize The maximum allowed size for the request headers in bytes.
 *
 * @note Updates m_isFirstLineParsed member.
 * @note Updates m_firstLineEndPos member.
 * @note Updates m_headersEndPos member.
 * @note Updates m_host member.
 *
 * @throws Http400Exception if the request line is malformed or if the Host header is missing when headers are complete.
 *
 * @return true if the headers are complete, false otherwise.
 *
 */
bool HttpRequest::isCompleteHeader()
{
	if (!m_isFirstLineParsed)
	{
		const size_t lineEnd = m_rawStr.find(HEADER_LINE_ENDING);
		if (lineEnd == std::string::npos)
			return false;
		try
		{
			m_firstLineEndPos	= parseFirstLine(lineEnd);
			m_isFirstLineParsed = true;
		}
		catch (const std::invalid_argument &e)
		{
			throw Http400Exception(e.what());
		}
	}
	const size_t headersEnd = m_rawStr.find(HEADER_ENDING);
	if (!m_host.first)
	{
		const std::string host = readHost(m_rawStr);
		if (host.empty())
		{
			if (headersEnd != std::string::npos)
				throw Http400Exception(REQ_MISSING_HOST_MSG);
			return false;
		}
		m_host = std::make_pair(true, host);
	}
	if (headersEnd == std::string::npos)
		return false;
	m_headersEndPos = headersEnd;
	return true;
}



/**
 *
 * @brief Checks if the HTTP request body is complete based on the Content-Length header.
 *
 * @param maxBodySize The maximum allowed size for the request body in bytes.
 *
 * @throws Http413Exception if the Content-Length exceeds the specified maximum body size.
 *
 * @return true if the request body is complete, false otherwise.
 *
 */
bool HttpRequest::isCompleteBody(const size_t maxBodySize)
{
	const HttpMethod::Type methodType = m_method.getType();
	if (methodType != HttpMethod::POST &&
		methodType != HttpMethod::PUT &&
		methodType != HttpMethod::PATCH)
		return true;
	const size_t contentLength = readContentLength(m_rawStr);
	if (contentLength > maxBodySize)
		throw Http413Exception(REQ_MAX_BODY_REACHED_MSG + Number::toString(maxBodySize) + " bytes");
	m_totalLen = m_headersEndPos + std::strlen(HEADER_ENDING) + contentLength;
	return m_rawStr.size() >= m_totalLen;
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
	char		  buffer[READ_CLIENT_BUFFER_SIZE];
	const ssize_t bytesRead = recv(socketFd, buffer, READ_CLIENT_BUFFER_SIZE - 1, MSG_DONTWAIT);

	if (bytesRead < 0)
		return -1;
	m_rawStr.append(buffer, bytesRead);
	return bytesRead;
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
