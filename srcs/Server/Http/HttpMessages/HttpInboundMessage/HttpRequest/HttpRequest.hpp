#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include "Vec.hpp"
#include "Config.hpp"
#include "AHttpInboundMessage.hpp"
#include "String.hpp"
#include "Accept.hpp"

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

		size_t parseFirstLine();
		void   parseTargetQuery(const std::string &path);
		void   parseActiveLocations(const Config &config);
		void   parseAccepts(const std::string &headerValue);
		bool   determineConnectionKeepAlive() const;

		ssize_t		 readContentLength(const std::string &request);
		std::string	 readMethodString(const std::string &currentRawStr) const;
		HttpRequest &parseCookie();


	public:
		HttpRequest();

		std::string							 getQueryString() const;
		size_t								 getSegmentSize() const;
		const Vec<std::string>				&getSegments() const;
		const Vec<const Config::Location *> &getActiveLocations() const;
		const Vec<Accept>					&getAccepts() const;
		bool								 getActiveAutoIndex() const;
		bool								 getConnectionKeepAlive() const;


		bool		hasUpload() const;
		std::string getUploadPath() const;

		bool isAllowedMethod() const;

		HttpRequest &parse(const Config &serverConfig);
		HttpRequest &reset();

		ssize_t		readOnce(int socketFd);
		std::string getCookieValue(const std::string &key) const;
		bool		hasCookieValue(const std::string &key) const;

		std::string buildResourcePath(const ssize_t segmentIndex) const;
		bool		matchAccept(const std::string &contentType) const;

		bool isComplete();
		bool isHeadersEndPos() const;
};

std::ostream &operator<<(std::ostream &o, const HttpRequest &message);


#endif
