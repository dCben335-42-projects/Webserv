#ifndef HTPP_RESPONSE_CPP
#define HTPP_RESPONSE_CPP

#include "AHttpMessage.hpp"
#include <ostream>

class HttpResponse : public AHttpMessage
{
    private:
        unsigned short  m_statusCode;
    public:
        HttpResponse();
        ~HttpResponse();

        unsigned short      getStatusCode() const;
        std::string         getHead() const;
        void                setStatusCode(unsigned short code);
        void                sendResponse(int clientSocket);
};

std::ostream &operator<<(std::ostream &o, HttpResponse const &response);

#endif