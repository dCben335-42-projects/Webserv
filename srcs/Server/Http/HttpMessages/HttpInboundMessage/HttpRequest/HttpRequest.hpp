#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include "Vec.hpp"
#include "Config.hpp"
#include "AHttpInboundMessage.hpp"

class HttpRequest : public AHttpInboundMessage
{
	private:
		std::string					  m_queryString;
		Vec<std::string>			  m_segments;
		Vec<const Config::Location *> m_activeLocations;

		size_t m_firstLineEndPos;
		size_t m_headersEndPos;
		size_t m_totalLen;
		bool   m_isFirstLineParsed;

		size_t parseFirstLine();
		void   parseTargetQuery(const std::string &path);
		void   parseActiveLocations(const Config &config);

	public:
		HttpRequest();

		std::string							 getQueryString() const;
		size_t								 getSegmentSize() const;
		const Vec<std::string>				&getSegments() const;
		const Vec<const Config::Location *> &getActiveLocations() const;

		HttpRequest &parse(const Config &serverConfig);
		HttpRequest &reset();


		ssize_t		readOnce(int socketFd);
		ssize_t		readContentLength(const std::string &request);
		std::string readMethodString(const std::string &currentRawStr) const;

		std::string buildResourcePath(const ssize_t segmentIndex) const;
		bool		getActiveAutoIndex() const;
		bool		isAllowedMethod() const;
		bool		hasUpload() const;
		std::string getUploadPath() const;

		bool isComplete();
};

std::ostream &operator<<(std::ostream &o, const HttpRequest &message);


#endif
