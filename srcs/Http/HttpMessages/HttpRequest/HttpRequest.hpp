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
        std::string                         m_rawStr;
        std::string                         m_target;

        std::string     readHeaders();
        size_t          readContentLength(std::string &request,const size_t headersEnd);
        std::string     readBody(const size_t contentLength, size_t received);
        size_t          parseFirstLine(const std::string &rawRequest);
        size_t          parseHeaderLine(const std::string &rawRequest, const size_t pos);

    public:
        HttpRequest(int m_socket);
        ~HttpRequest();
        
        std::string     getRawRequest() const;
        std::string     getTarget() const;

        void            setRawRequest(const std::string &rawRequest);
        void            setTarget(const std::string &path);
        
        HttpRequest&    read();
        HttpRequest&    parse();
};

#endif
