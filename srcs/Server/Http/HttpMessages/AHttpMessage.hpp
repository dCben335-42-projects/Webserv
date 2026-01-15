#ifndef A_HTTP_MESSAGE_HPP
#define A_HTTP_MESSAGE_HPP

#include "HttpMethod.hpp"
#include "Map.hpp"

class AHttpMessage
{
	protected:
		Map<std::string> m_headers;
		HttpMethod		 m_method;
		std::string		 m_body;

	public:
		AHttpMessage();
		~AHttpMessage();

		Map<std::string> getHeaders() const;
		std::string		 getHeader(const std::string &key) const;
		std::string		 getMethod() const;
		std::string		 getBody() const;

		void setHeader(const std::string &key, const std::string &value);
		void setMethod(const HttpMethod &method);
		void setBody(const std::string &body);

		void appendBody(const std::string &data);
		void appendBody(const char *buffer, size_t size);
};

std::ostream &operator<<(std::ostream &o, AHttpMessage const &message);

#endif