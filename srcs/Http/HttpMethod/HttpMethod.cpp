#include "HttpMethod.hpp"
#include <stdexcept>
#include "config.hpp"
#include "httpErrors.hpp"

const Map<EMethodType, std::string> HttpMethod::m_methodMap = HttpMethod::initMethodMap();


Map<EMethodType, std::string> HttpMethod::initMethodMap()
{
	Map<EMethodType, std::string> methodMap;

	methodMap[GET]	  = M_GET;
	methodMap[POST]	  = M_POST;
	methodMap[DELETE] = M_DELETE;
	methodMap[PUT]	  = M_PUT;
	methodMap[PATCH]  = M_PATCH;
	return methodMap;
}

HttpMethod::HttpMethod() : m_type(NONE)
{
}

HttpMethod::HttpMethod(const std::string &method)
{
	Map<EMethodType, std::string>::const_iterator it  = m_methodMap.begin();
	Map<EMethodType, std::string>::const_iterator end = m_methodMap.end();

	for (; it != end; ++it)
	{
		if (it->second == method)
		{
			m_type	  = it->first;
			m_strType = it->second;
			return;
		}
	}
	throw Http501Exception(method);
}

HttpMethod::HttpMethod(const EMethodType &method)
{
	Map<EMethodType, std::string>::const_iterator it = m_methodMap.find(method);

	if (it != m_methodMap.end())
	{
		m_type	  = it->first;
		m_strType = it->second;
		return;
	}
	throw Http501Exception("adawdaw");
}

HttpMethod &HttpMethod::operator=(const HttpMethod &other)
{
	if (this == &other)
		return *this;
	m_type	  = other.m_type;
	m_strType = other.m_strType;
	return *this;
}

HttpMethod::~HttpMethod()
{
}

EMethodType HttpMethod::getType() const
{
	return m_type;
}

std::string HttpMethod::getStrType() const
{
	return m_strType;
}