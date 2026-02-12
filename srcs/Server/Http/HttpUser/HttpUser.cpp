#include "HttpUser.hpp"
#include "Number.hpp"
#include "webserv.hpp"
#include "String.hpp"
#include "httpExceptions.hpp"
#include "Date.hpp"
#include <cstdlib>
#include "Logger.hpp"

HttpCookie::HttpCookie(
	const std::string name,
	const std::string pswd,
	const std::string serverName,
	unsigned short	  serverPort) :
	m_uname(name),
	m_pswd(pswd),
	m_serverName(serverName),
	m_serverPort(serverPort),
	m_stringifiedCookie(""),
	m_salt("")
{
	m_creationDate	 = std::time(0);
	m_expirationDate = m_creationDate + COOKIE_EXPIRE_DURATION;
	generateEncryptedPassword();
	m_hashInt = computeHash(m_uname + m_serverName + Number::toString(m_serverPort) + Number::toString(m_creationDate));
	m_hashStr = Number::toString(m_hashInt);
	Logger::error(m_pswd);
}

HttpCookie::HttpCookie() :
	m_uname(""),
	m_pswd(""),
	m_serverName(""),
	m_serverPort(0),
	m_stringifiedCookie(""),
	m_hashStr(""),
	m_hashInt(0),
	m_salt("")
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

const std::string &HttpCookie::getSalt() const
{
	return m_salt;
}

const time_t &HttpCookie::getExpDate() const
{
	return m_expirationDate;
}

bool HttpCookie::operator==(const HttpCookie &obj) const
{
	return m_hashStr == obj.m_hashStr;
}

uint64_t HttpCookie::computeHash(const std::string str) const
{
	uint64_t hash = FNV_OFFSET_BASIS_64;

	for (std::string::size_type i = 0; i < str.size(); i++)
	{
		hash ^= static_cast<unsigned char>(str[i]);
		hash *= FNV_PRIME_64;
	}
	hash ^= (hash >> MURMUR_FIRST_SHIFT);
	hash *= MURMUR_HASH;
	hash ^= (hash >> MURMUR_LAST_SHIFT);
	return hash;
}

void HttpCookie::parse(std::string rawCookie)
{
	Vec<std::string> keyValues = String::splitPattern(rawCookie, COOKIE_FIELD_SEP);
	Map<std::string> fields;
	for (Vec<std::string>::iterator it = keyValues.begin(); it != keyValues.end(); it++)
	{
		size_t equalIndex = it->find(COOKIE_VALUE_SEP);
		if (equalIndex == std::string::npos)
			throw Http401Exception();
		fields.set(it->substr(0, equalIndex), it->substr(equalIndex + 1));
	}
	if (!fields.has(COOKIE_KEY_TOKEN))
		throw Http401Exception();
	m_hashStr = fields.get(COOKIE_KEY_TOKEN);
}

void HttpCookie::stringifyCookie()
{
	m_stringifiedCookie = COOKIE_KEY_TOKEN									 COOKIE_VALUE_SEP + m_hashStr;
	m_stringifiedCookie += COOKIE_FIELD_SEP COOKIE_KEY_PATH COOKIE_VALUE_SEP COOKIE_VALUE_PATH;
	m_stringifiedCookie += COOKIE_FIELD_SEP COOKIE_KEY_EXPIRES				 COOKIE_VALUE_SEP + Date::toHttpDateFormat(gmtime(&m_expirationDate));
	m_stringifiedCookie += COOKIE_FIELD_SEP									 COOKIE_KEY_HTTPONLY;
}

void HttpCookie::generateEncryptedPassword()
{
	uint32_t salt		= std::rand();
	uint32_t secondSalt = std::rand();

	m_salt = Number::toString(salt) + Number::toString(secondSalt);
	m_pswd = Number::toString(computeHash(m_pswd + m_salt));
}
