
#ifndef HTTPCLIENT_HPP
#define HTTPCLIENT_HPP

#include "HttpRequest.hpp"



class HttpClient
{
    private:
        HttpRequest  m_request;
        
    public:
        HttpClient();
        ~HttpClient();
};

HttpClient::HttpClient()
{


}

HttpClient::~HttpClient()
{
}

#endif
