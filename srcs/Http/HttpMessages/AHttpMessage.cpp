#include "AHttpMessage.hpp"
#include "Number.hpp"
#include "config.hpp"
#include <exception>
#include "Logger.hpp"

AHttpMessage::AHttpMessage()
{
}

AHttpMessage::~AHttpMessage()
{
}



// ANCHOR Getters
Map<std::string> AHttpMessage::getHeaders() const
{
	return m_headers;
}
std::string AHttpMessage::getHeader(const std::string &key) const
{
	Map<std::string>::const_iterator it = m_headers.find(key);
	return it != m_headers.end() ? it->second : "";
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
void AHttpMessage::setHeader(const std::string &key, const std::string &value)
{
	m_headers[key] = value;
}
void AHttpMessage::setBody(const std::string &body)
{
	m_body							 = body;
	m_headers[HEADER_CONTENT_LENGTH] = Number::toString(body.size());
}
void AHttpMessage::setMethod(const HttpMethod &method)
{
	m_method = method;
}





// ANCHOR Overload operator <<
std::ostream &operator<<(std::ostream &o, AHttpMessage const &message)
{
	Map<std::string> headers = message.getHeaders();
	// Map<std::string>::iterator it	   = headers.begin();
	// const size_t total = headers.size();
	// size_t index = 0;

	// log("Headers:");
	log(headers);
	// while (it != headers.end())
	// {
	// 	o << "\"" << it->first << "\""
	// 	  << ": \"" << it->second << "\"";
	// 	if (index < total - 1)
	// 		o << "\n";
	// 	it++;
	// 	index++;
	// }
	return o;
}
