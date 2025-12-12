#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <map>
#include "AHttpMessage.hpp"
#include <sys/types.h>
#include <sys/socket.h>

class HttpRequest : public AHttpMessage
{
	private:
		std::string						   m_body;
		std::map<std::string, std::string> m_headers;
		std::string						   m_method;
		std::string						   m_rawStr;
		std::string						   m_target;

		std::string readHeaders();
		std::string readBody(const size_t contentLength, size_t received);
		size_t		parseFirstLine(const std::string &rawRequest);
		size_t		parseHeaderLine(const std::string &rawRequest, const size_t pos);

	public:
		HttpRequest();
		~HttpRequest();

		std::string getRawRequest() const;
		std::string getTarget() const;

		void setRawRequest(const std::string &rawRequest);
		void setTarget(const std::string &path);

		HttpRequest &read();
		HttpRequest &parse();

		ssize_t readOnce(int socketFd);
		size_t	readContentLength(std::string request, const size_t headersEnd);
};

std::ostream &operator<<(std::ostream &o, const HttpRequest &message);


#endif
