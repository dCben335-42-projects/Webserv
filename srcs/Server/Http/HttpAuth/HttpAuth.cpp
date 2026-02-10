#include "HttpAuth.hpp"
#include "String.hpp"
#include "httpExceptions.hpp"
#include "Logger.hpp"

HttpAuth::HttpAuth()
{
}

HttpAuth::~HttpAuth()
{
}

void HttpAuth::parse(const std::string &body)
{
	Vec<std::string> bodySplit = String::split(body, "&");

	for (Vec<std::string>::iterator it = bodySplit.begin(); it != bodySplit.end(); it++)
	{
		Vec<std::string> fieldSplit = String::split((*it), "=");
		if (fieldSplit.size() != 2)
			throw Http400Exception();
		m_urlEncoded.set(fieldSplit[0], fieldSplit[1]);
	}
	checkField(AUTH_NAME_KEY);
	checkField(AUTH_PSWD_KEY);
	checkField(AUTH_CONFIRM_KEY);
	if (m_urlEncoded.get(AUTH_PSWD_KEY) != m_urlEncoded.get(AUTH_CONFIRM_KEY))
		throw Http400Exception();
}

void HttpAuth::checkField(const std::string field)
{
	if (!m_urlEncoded.has(field))
		throw Http400Exception();
	if (m_urlEncoded.get(field).empty())
		throw Http400Exception();
}
