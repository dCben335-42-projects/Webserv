#include "AHttpMessage.hpp"
#include <exception>
#include "config.hpp"
#include "Number.hpp"

AHttpMessage::AHttpMessage(void) :
    m_method(Method(GET)) {}
AHttpMessage::AHttpMessage(const std::string &method) :
    m_method(Method(method))
{
}

AHttpMessage::AHttpMessage(const EMethodType &method) :
    m_method(Method(method))
{
}

AHttpMessage::~AHttpMessage() {}


// ANCHOR Getters
std::map<std::string, std::string> AHttpMessage::getHeaders() const
{
    return m_headers;
}
std::string AHttpMessage::getHeader(const std::string &key) const
{
    std::map<std::string, std::string>::const_iterator it = m_headers.find(key);
    return it !=  m_headers.end() ? it->second : ""; 
}
std::string AHttpMessage::getMethod() const
{
    return m_method.getStrType();
}
std::string AHttpMessage::getBody() const 
{
    return m_body;
}



// ANCHOR Setters
void    AHttpMessage::setHeader(const std::string &key, const std::string &value) 
{
    m_headers[key] = value;
}
void    AHttpMessage::setBody(const std::string &body)
{
    m_body = body;
    m_headers[HEADER_CONTENT_LENGTH] = Number::toString(body.size());
}
void    AHttpMessage::setMethod(const Method &method)
{
    m_method = method;
}


// ANCHOR Overload operator <<
std::ostream &operator<<(std::ostream &o, AHttpMessage const &message)
{
    std::map<std::string, std::string> headers = message.getHeaders();
    std::map<std::string, std::string>::iterator it = headers.begin();
    size_t total = headers.size();
    size_t index = 0;
    
    while (it != headers.end())
    {
        o << it->first << ": " << it->second;
        if (index < total - 1)
            o << "\n";
        it++;
        index++;
    }
	return o;
}
