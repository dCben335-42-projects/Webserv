#ifndef HTPP_RESPONSE_CPP
#define HTPP_RESPONSE_CPP

#include "AHttpMessage.hpp"

class HttpResponse : public AHttpMessage
{
	private:
		std::string m_fullResponse;
		size_t		m_sentBytes;



	public:
		HttpResponse();
		~HttpResponse();

		unsigned short getStatusCode() const;
		std::string	   getFullResponse() const;
		size_t		   getSentBytes() const;

		void setStatusCode(unsigned short code);
		void setFullResponse(const std::string str);
		void appendToFullResponse(const char *buf, size_t size);

		bool sendOnce(int socket);

		HttpResponse &build();
		HttpResponse &reset();
		HttpResponse &fill(bool shouldClose);
};

#endif
