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
	Logger::log(m_urlEncoded);
}
