#ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

#include <string>
#include "Url.hpp"

class HttpServer
{
    private:
        int             m_port;
        std::string     m_address;
        UrlT<void>      m_url;
        
    public:
        HttpServer();
        ~HttpServer();
};

#endif
