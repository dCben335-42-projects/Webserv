#include "HttpSubRequest.hpp"
#include "Logger.hpp"
#include "config.hpp"

HttpSubRequest::HttpSubRequest(const std::string &rawStr)
{
	m_rawStr = String::trimBegin(rawStr);
	size_t i = 0;
	parse(m_rawStr, i);
	m_headers.remove(HEADER_CONTENT_LENGTH);
}

HttpSubRequest::~HttpSubRequest()
{
}
