#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include "AHttpInboundMessage.hpp"
#include "Config.hpp"
#include "Location.hpp"
#include "Accept.hpp"

#define REQ_MISSING_HOST_MSG "Missing Host Header Field"
#define REQ_INVALID_CONTENT_LENGTH_MSG "Invalid Content-Length value: No digits found"
#define REQ_CONTENT_LENGTH_MISMATCH_MSG "Content-Length header value does not match the actual body size"
#define REQ_UNSUPPORTED_METHOD_MSG "Unsupported HTTP Version: "
#define REQ_INVALID_TARGET_MSG "Invalid Request Target: "
#define REQ_MISSING_CONTENT_LENGTH_MSG "Missing Content-Length Header"
#define REQ_EXPECTED_HEADER_COLON_MSG "Expected ':' after header designation"

#define REQ_SERVER_MAX_HEADERS_REACHED_MSG "The request headers size exceeds the server's configured maximum of "
#define REQ_DEFAULT_MAX_HEADERS_REACHED_MSG "The request headers size exceeds the default maximum of "
#define REQ_MAX_BODY_REACHED_MSG "The request body size exceeds the server's configured maximum of "

class HttpRequest : public AHttpInboundMessage
{
	private:
		std::string					  m_queryString;
		Vec<std::string>			  m_segments;
		Vec<const Config::Location *> m_activeLocations;
		Vec<Accept>					  m_accepts;
		bool						  m_connectionKeepAlive;
		Map<std::string>			  m_cookieValues;

		size_t m_firstLineEndPos;
		size_t m_headersEndPos;
		size_t m_totalLen;
		bool   m_isFirstLineParsed;

		std::pair<bool, std::string> m_host;


		size_t parseFirstLine();
		void   parseTargetQuery(const std::string &path);
		void   parseActiveLocations(const Config &config);
		void   parseAccepts(const std::string &headerValue);
		bool   determineConnectionKeepAlive() const;

		size_t		 readContentLength(const std::string &headers) const;
		std::string	 readMethodString(const std::string &currentRawStr) const;
		std::string	 readHost(const std::string &headers) const;
		HttpRequest &parseCookie();

		void checkHeadersSize(const size_t maxHeadersSize) const;
		bool isCompleteHeader(const size_t maxHeadersSize);
		bool isCompleteBody(const size_t maxBodySize);

		typedef Vec<const Config::Location *>::const_iterator m_locCIt;

	public:
		HttpRequest();

		std::string							 getQueryString() const;
		size_t								 getSegmentSize() const;
		const Vec<std::string>				&getSegments() const;
		const Vec<const Config::Location *> &getActiveLocations() const;
		const Vec<Accept>					&getAccepts() const;
		bool								 getActiveAutoIndex() const;
		bool								 getConnectionKeepAlive() const;
		const Set<HttpMethod>				*getAllowedMethods() const;
		bool								 hasHost() const;
		const std::string					&getHost() const;

		bool		hasUpload() const;
		std::string getUploadPath() const;

		bool				  hasReturn() const;
		const LocationReturn &getReturn() const;

		bool isAllowedMethod() const;

		HttpRequest &parse(const Config &serverConfig);
		HttpRequest &reset();

		ssize_t		readOnce(int socketFd);
		std::string getCookieValue(const std::string &key) const;
		bool		hasCookieValue(const std::string &key) const;

		std::string buildResourcePath(const ssize_t segmentIndex) const;
		bool		matchAccept(const std::string &contentType) const;

		bool isComplete(const size_t maxBodySize, const size_t maxHeadersSize);
		bool hasCompleteHeader() const;
};

std::ostream &operator<<(std::ostream &o, const HttpRequest &message);


#endif
