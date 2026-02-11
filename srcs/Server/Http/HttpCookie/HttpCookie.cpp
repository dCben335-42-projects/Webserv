#include "HttpCookie.hpp"
#include "Number.hpp"
#include "webserv.hpp"
#include "String.hpp"
#include "httpExceptions.hpp"
#include "Date.hpp"

HttpCookie::HttpCookie(
	const std::string name,
	const std::string pswd,
	const std::string serverName,
	unsigned short	  serverPort) :
	m_uname(name),
	m_pswd(pswd),
	m_serverName(serverName),
	m_serverPort(serverPort),
	m_stringifiedCookie("")
{
	m_creationDate	 = std::time(0);
	m_expirationDate = m_creationDate + COOKIE_EXPIRE_DURATION;
	m_hashInt		 = computeHash(m_uname + m_serverName + Number::toString(m_serverPort) + Number::toString(m_creationDate));
	m_hashStr		 = Number::toString(m_hashInt);
}

HttpCookie::HttpCookie() :
	m_uname(""),
	m_pswd(""),
	m_serverName(""),
	m_serverPort(0),
	m_stringifiedCookie(""),
	m_hashStr(""),
	m_hashInt(0)
{
	m_creationDate	 = std::time(0);
	m_expirationDate = m_creationDate + COOKIE_EXPIRE_DURATION;
}

HttpCookie::~HttpCookie()
{
}

const std::string &HttpCookie::getStringifiedCookie() const
{
	return m_stringifiedCookie;
}

const std::string &HttpCookie::getName() const
{
	return m_uname;
}

const std::string &HttpCookie::getPswd() const
{
	return m_pswd;
}

bool HttpCookie::operator==(const HttpCookie &obj) const
{
	if (m_hashStr == obj.m_hashStr)
		return true;
	return false;
}

unsigned int HttpCookie::computeHash(const std::string str)
{
	unsigned int hash = FNV_OFFSET_BASIS_32;

	for (std::string::size_type i = 0; i < str.size(); i++)
	{
		hash ^= static_cast<unsigned char>(str[i]);
		hash *= FNV_PRIME_32;
	}
	hash ^= (hash >> MURMUR_FIRST_SHIFT);
	hash *= MURMUR_HASH;
	hash ^= (hash >> MURMUR_LAST_SHIFT);
	return hash;
}

#include "Logger.hpp"
void HttpCookie::parse(std::string rawCookie)
{
	Vec<std::string> keyValues = String::splitPattern(rawCookie, "; ");
	Map<std::string> fields;
	for (Vec<std::string>::iterator it = keyValues.begin(); it != keyValues.end(); it++)
	{
		size_t equalIndex = it->find("=");
		if (equalIndex == std::string::npos)
			throw Http401Exception();
		fields.set(it->substr(0, equalIndex), it->substr(equalIndex + 1));
	}
	if (!fields.has("token"))
		throw Http401Exception();
	m_hashStr = fields.get("token");
}

void HttpCookie::stringifyCookie()
{
	m_stringifiedCookie = "token=" + m_hashStr + "; " + "path=/; " + "expires=" + Date::toHttpDateFormat(gmtime(&m_expirationDate)) + "; " + "HttpOnly";
}