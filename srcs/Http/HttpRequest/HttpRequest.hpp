#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <map>

class HttpRequest
{
    private:
        std::string                         m_body;
        std::map<std::string, std::string>  m_headers;
        std::string                        	m_method;
        std::string                        	m_url;
        std::string                        	m_protocol;
    public:
        HttpRequest();
        ~HttpRequest();
};

#endif
