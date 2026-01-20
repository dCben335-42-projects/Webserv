#ifndef HTPP_RESPONSE_CPP
#define HTPP_RESPONSE_CPP

#include "AHttpMessage.hpp"
#include <ostream>
#include <Map.hpp>

class HttpResponse : public AHttpMessage
{
	private:
		unsigned short m_statusCode;
		std::string	   m_fullResponse;
		size_t		   m_sentBytes;

		static const Map<unsigned short, std::string> m_statusReasonMap;

		static Map<unsigned short, std::string> initStatusReasonMap();

	public:
		HttpResponse();
		~HttpResponse();

		unsigned short getStatusCode() const;
		std::string	   getFullResponse() const;
		size_t		   getSentBytes() const;

		void setStatusCode(unsigned short code);
		void setFullResponse(const std::string str);
		void appendToFulFullResponse(const char *buf, size_t size);

		bool sendOnce(int socket);

		void		build();
		std::string buildHead() const;
		void		reset();
};

std::ostream &operator<<(std::ostream &o, HttpResponse const &response);

#endif