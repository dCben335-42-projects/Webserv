#ifndef A_HTTP_MESSAGE_HPP
#define A_HTTP_MESSAGE_HPP

#include "HttpMethod.hpp"
#include "httpHeaders.hpp"
#include "Map.hpp"

class AHttpMessage
{
	protected:
		Map<std::string> m_headers;
		HttpMethod		 m_method;
		std::string		 m_body;
		unsigned short	 m_statusCode;

		static const Map<unsigned short, std::string> m_statusReasonMap;

		static Map<unsigned short, std::string> initStatusReasonMap();

		std::string buildHead() const;
		std::string build();

	public:
		AHttpMessage();
		~AHttpMessage();

		const Map<std::string> &getHeaders() const;
		std::string				getHeader(const std::string &key) const;
		const HttpMethod	   &getMethod() const;
		std::string				getBody() const;

		void setHeader(const std::string &key, const std::string &value);
		void setMethod(const HttpMethod &method);
		void setBody(const std::string &body);

		void appendBody(const std::string &data);
		void appendBody(const char *buffer, size_t size);
};

std::ostream &operator<<(std::ostream &o, AHttpMessage const &message);

#endif
