#include "HttpCookie.hpp"
#include "Number.hpp"
#include "webserv.hpp"
#include "String.hpp"

HttpCookie::HttpCookie(
	const std::string name,
	const std::string pswd,
	const std::string serverName,
	unsigned short	  serverPort) :
	m_uname(name),
	m_pswd(pswd),
	m_serverName(serverName),
	m_serverPort(serverPort)
{
	m_hashInt = computeHash(m_uname + m_serverName + Number::toString(m_serverPort));
	m_hashStr = Number::toString(m_hashInt);
}

HttpCookie::~HttpCookie()
{
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
