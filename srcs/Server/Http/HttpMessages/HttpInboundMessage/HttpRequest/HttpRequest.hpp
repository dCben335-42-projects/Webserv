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

		size_t parseFirstLine(const std::string &rawRequest);
		void   parseTargetQuery(const std::string &path);
		void   parseActiveLocations(const Config &config);

	public:
		HttpRequest();
		~HttpRequest();


		std::string							 getQueryString() const;
		size_t								 getSegmentSize() const;
		const Vec<std::string>				&getSegments() const;
		const Vec<const Config::Location *> &getActiveLocations() const;


		HttpRequest &parse(const Config &serverConfig);

		ssize_t readOnce(int socketFd);
		size_t	readContentLength(std::string request, const size_t headersEnd);

		std::string buildResourcePath(const ssize_t segmentIndex) const;
};

std::ostream &operator<<(std::ostream &o, const HttpRequest &message);


#endif
