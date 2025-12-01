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

        std::string     readHeaders(const int clientSocket);
        size_t          readContentLength(
                            std::string &request,
                            const size_t headersEnd
                        );
        std::string     readBody(
                            const int clientSocket,
                            const size_t contentLength,
                            size_t received
                        );

        size_t          parseMethod(const std::string &rawRequest);
        size_t          parseHeaderLine(
                            const std::string &rawRequest,
                            const size_t pos
                        );

    public:
        HttpRequest();
        ~HttpRequest();
        
        std::string getRawRequest() const;
        std::string setRawRequest(const std::string &rawRequest);
        
        HttpRequest&    read(const int clientSocket);
        HttpRequest&    parse();
};

#endif
