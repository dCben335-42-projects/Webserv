#ifndef A_HTTP_MESSAGE_HPP
#define A_HTTP_MESSAGE_HPP

#include "HttpMethod.hpp"
#include "httpHeaders.hpp"
#include "Map.hpp"
#include "Vec.hpp"

class AHttpMessage
{
	protected:
		Map<std::string> m_headers;
		HttpMethod		 m_method;
		std::string		 m_body;
		unsigned short	 m_statusCode;

		static const Map<unsigned short, std::string> m_statusReasonMap;
		static const Vec<unsigned short>			  m_closeStatus;
		static const Vec<unsigned short>			  m_redirectionStatus;

		static Map<unsigned short, std::string> initStatusReasonMap();
		static Vec<unsigned short>				initCloseStatus();
		static Vec<unsigned short>				initRedirectionStatus();

		std::string buildHead() const;
		std::string build();

	public:
		AHttpMessage();
		~AHttpMessage();

		const Map<std::string> &getHeaders() const;
		std::string				getHeader(const std::string &key) const;
		const HttpMethod	   &getMethod() const;
		const std::string	   &getBody() const;

		void setMethod(const HttpMethod &method);
		void setBody(const std::string &body);

		bool hasHeader(const std::string &key) const;
		void setHeader(const std::string &key, const std::string &value);
		void deleteHeader(const std::string &key);

		bool isCloseStatus(unsigned short statusCode) const;
		bool isRedirectionStatus(unsigned short statusCode) const;
};

std::ostream &operator<<(std::ostream &o, AHttpMessage const &message);

#endif
