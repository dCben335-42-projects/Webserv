#ifndef HTPP_RESPONSE_CPP
#define HTPP_RESPONSE_CPP

#include <string>
#include "Url.hpp"
#include <map>

class HttpResponse
{
    private:
        std::string                         m_body;
        std::map<std::string, std::string>  m_headers;
        int                                 m_statusCode;
        std::string                         m_protocol;
    public:
        HttpResponse();
        ~HttpResponse();

        std::string getBody() const;
        int getStatusCode() const;
        std::string getProtocol() const;
        std::string getHeader(const std::string& key) const;

        void setBody(const std::string& body);
        void setStatusCode(int statusCode);
        void setProtocol(const std::string& protocol);
        void setHeader(const std::string& key, const std::string& value);

        void sendResponse(int clientSocket);
};

#endif