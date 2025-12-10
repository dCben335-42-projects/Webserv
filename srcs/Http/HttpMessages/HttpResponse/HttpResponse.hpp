#ifndef HTPP_RESPONSE_CPP
#define HTPP_RESPONSE_CPP

#include "AHttpMessage.hpp"
#include <ostream>

class HttpResponse : public AHttpMessage
{
    private:
        unsigned short	m_statusCode;
        std::string		m_fullResponse;
        size_t          m_sentBytes;


    public:
        HttpResponse();
        ~HttpResponse();

        unsigned short      getStatusCode() const;
		std::string			getFullResponse() const;
		size_t				getSentBytes() const;
		
        void                setStatusCode(unsigned short code);

        bool                sendRes(int socket);
		void				build();
		void                reset();

		std::string		buildHead() const;

};

std::ostream &operator<<(std::ostream &o, HttpResponse const &response);

#endif