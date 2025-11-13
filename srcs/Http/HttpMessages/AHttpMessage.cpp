#include "AHttpMessage.hpp"
#include <exception>


AHttpMessage::AHttpMessage(const std::string &method) :
    m_method(Method(method))
{
}

AHttpMessage::~AHttpMessage() {}



std::string AHttpMessage::getHeader(const std::string &key) const
{
    std::map<std::string, std::string>::const_iterator it = m_headers.find(key);
    return it !=  m_headers.end() ? it->second : ""; 
}



void    AHttpMessage::setHeader(const std::string &key, const std::string &value) 
{
    m_headers[key] = value;
}
void    AHttpMessage::setBody(const std::string &body) 
{
    m_body = body;
}
void    AHttpMessage::getMethod(void)
{
    return 
}