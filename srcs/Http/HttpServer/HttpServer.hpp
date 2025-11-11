#ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

#include <string>
#include "Url.hpp"

class HttpServer
{
    private:
        Url<void>       m_url;
        
        
    public:
        HttpServer();
        ~HttpServer();
};

#endif
