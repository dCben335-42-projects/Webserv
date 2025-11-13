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
    public:
        HttpResponse();
        ~HttpResponse();

        std::string getBody() const;
        int getStatusCode() const;
        std::string getHeader(const std::string& key) const;

        void setBody(const std::string& body);
        void setStatusCode(int statusCode);
        void setHeader(const std::string& key, const std::string& value);

        void sendResponse(int clientSocket);
};

#endif