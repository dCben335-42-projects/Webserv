#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <map>
#include "AHttpMessage.hpp"

class HttpRequest : public AHttpMessage
{
    private:
        std::string                         m_body;
        std::map<std::string, std::string>  m_headers;
        std::string                        	m_method;
    public:
        HttpRequest();
        ~HttpRequest();
};

#endif
