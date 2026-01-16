#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <map>
#include "AHttpMessage.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include "Vector.hpp"

class HttpRequest : public AHttpMessage
{
	private:
		std::string		 m_rawStr;
		std::string		 m_target;
		std::string		 m_queryString;
		Vec<std::string> m_segments;

		size_t parseFirstLine(const std::string &rawRequest);
		size_t parseHeaderLine(const std::string &rawRequest, const size_t pos);
		void   parseTargetQuery(const std::string &path);

	public:
		HttpRequest();
		~HttpRequest();

		std::string		 getRawRequest() const;
		std::string		 getTarget() const;
		std::string		 getQueryString() const;
		Vec<std::string> getSegments() const;

		void setRawRequest(const std::string &rawRequest);

		HttpRequest &read();
		HttpRequest &parse();

		ssize_t readOnce(int socketFd);
		size_t	readContentLength(std::string request, const size_t headersEnd);
};

std::ostream &operator<<(std::ostream &o, const HttpRequest &message);


#endif
