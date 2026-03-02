#include "HttpUser.hpp"
#include "Number.hpp"
#include "webserv.hpp"
#include "String.hpp"
#include "httpExceptions.hpp"
#include "Date.hpp"
#include "Logger.hpp"

HttpUser::HttpUser(
	const std::string name,
	const std::string pswd,
	unsigned short	  serverPort) :
	m_uname(name),
	m_salt(""),
	m_serverPort(serverPort),
	m_stringifiedCookie(""),
	m_token(AuthToken(m_uname, m_serverPort))
{
	m_creationDate = std::time(0);
	m_pswd		   = generateEncryptedPassword(pswd);
}

HttpUser::~HttpUser()
{
}

const std::string &HttpUser::getStringifiedCookie() const
{
	return m_stringifiedCookie;
}

const std::string &HttpUser::getName() const
{
	return m_uname;
}

const std::string &HttpUser::getPswd() const
{
	return m_pswd;
}

const std::string &HttpUser::getSalt() const
{
	return m_salt;
}

const AuthToken &HttpUser::getToken() const
{
	return m_token;
}

void HttpUser::stringifyCookie()
{
	time_t expDate = m_token.getExpDate();

	m_stringifiedCookie = COOKIE_KEY_TOKEN									 COOKIE_VALUE_SEP + m_token.getHashStr();
	m_stringifiedCookie += COOKIE_FIELD_SEP COOKIE_KEY_PATH COOKIE_VALUE_SEP COOKIE_VALUE_PATH;
	m_stringifiedCookie += COOKIE_FIELD_SEP COOKIE_KEY_EXPIRES				 COOKIE_VALUE_SEP + Date::toHttpDateFormat(gmtime(&expDate));
	m_stringifiedCookie += COOKIE_FIELD_SEP									 COOKIE_KEY_HTTPONLY;
}

std::string HttpUser::generateEncryptedPassword(const std::string &pswd)
{
	uint32_t	salt	   = std::rand();
	uint32_t	secondSalt = std::rand();
	std::string encryptedPass;

	m_salt		  = Number::toString(salt) + Number::toString(secondSalt);
	encryptedPass = Number::toString(m_token.computeHash(pswd + m_salt));
	return encryptedPass;
}

bool HttpUser::hasValidToken() const
{
	time_t expDate	  = m_token.getExpDate();
	time_t actualTime = std::time(0);
	return Date::getDiffTime(expDate, actualTime) > COOKIE_SESSION_MIN;
}

void HttpUser::refreshToken()
{
	AuthToken newToken(m_uname, m_serverPort);

	m_token = newToken;
	stringifyCookie();
}