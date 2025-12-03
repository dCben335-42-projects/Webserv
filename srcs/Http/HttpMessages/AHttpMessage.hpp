#ifndef A_HTTP_MESSAGE_HPP
# define A_HTTP_MESSAGE_HPP

# include "Method.hpp"
# include <string>
# include <map>
# include <iostream>

class AHttpMessage
{
    protected:
        const int                           m_socket;
        std::map<std::string, std::string>  m_headers;
        Method                              m_method;
        std::string                         m_body;

    public:
        AHttpMessage(int socket);
        ~AHttpMessage();

        std::map<std::string, std::string>  getHeaders() const;
        std::string getHeader(const std::string &key) const;
        std::string getMethod() const;
        std::string getBody() const;

        void setHeader(const std::string &key, const std::string &value);
        void setMethod(const Method &method);
        void setBody(const std::string &body);
};

std::ostream &operator<<(std::ostream &o, AHttpMessage const &message);

#endif