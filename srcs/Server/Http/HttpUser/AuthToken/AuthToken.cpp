#include "AuthToken.hpp"
#include "constants.hpp"
#include "webserv.hpp"
#include "Number.hpp"

AuthToken::AuthToken(const std::string &name, unsigned short serverPort)
{
	m_creationDate	 = std::time(0);
	m_expirationDate = m_creationDate + COOKIE_EXPIRE_DURATION;
	m_hashInt		 = computeHash(name + Number::toString(serverPort) + Number::toString(m_creationDate));
	m_hashStr		 = Number::toString(m_hashInt);
}

AuthToken::AuthToken(const std::string &hash) : m_hashStr(hash)
{
}

AuthToken::~AuthToken()
{
}

time_t AuthToken::getExpDate() const
{
	return m_expirationDate;
}

const std::string &AuthToken::getHashStr() const
{
	return m_hashStr;
}

bool AuthToken::operator==(const AuthToken &obj) const
{
	return m_hashStr == obj.m_hashStr;
}

void AuthToken::setHashStr(const std::string &newHash)
{
	m_hashStr = newHash;
}

uint64_t AuthToken::computeHash(const std::string str)
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
