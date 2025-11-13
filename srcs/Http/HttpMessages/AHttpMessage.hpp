#ifndef A_HTTP_MESSAGE_HPP
# define A_HTTP_MESSAGE_HPP

# include <string>
# include <map>
# include "Method.hpp"
# include <iostream>

class AHttpMessage
{
    protected:
        std::map<std::string, std::string>  m_headers;
        Method                              m_method;
        std::string                         m_body;

    public:
        AHttpMessage();
        AHttpMessage(const std::string &method);
        AHttpMessage(const EMethodType &method);
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