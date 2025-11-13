#ifndef HTPP_RESPONSE_CPP
#define HTPP_RESPONSE_CPP

#include <string>
#include "AHttpMessage.hpp"
#include <map>

class HttpResponse : public AHttpMessage
{
    private:
        unsigned short  m_statusCode;
    public:
        HttpResponse();
        ~HttpResponse();

        unsigned short      getStatusCode() const;
        void                setStatusCode(unsigned short code);
        std::string         getHead() const;
        void                sendResponse(int clientSocket);
};

std::ostream &operator<<(std::ostream &o, HttpResponse const &response);

#endif