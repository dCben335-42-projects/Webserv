#ifndef A_HTTP_MESSAGE_HPP
# define A_HTTP_MESSAGE_HPP

# include <string>
# include <map>
# include "Method.hpp"

class AHttpMessage {
    protected:
        std::map<std::string, std::string>  m_headers;
        const Method                        m_method;
        std::string                         m_body;

    public:
        AHttpMessage(const std::string &method);
        ~AHttpMessage();

        std::string getHeader(const std::string &key) const;
        std::string getMethod() const;
        std::string getBody() const;

        void setHeader(const std::string &key, const std::string &value);
        void setMethod(const std::string &method);
        void setBody(const std::string &body)
        
};

#endif