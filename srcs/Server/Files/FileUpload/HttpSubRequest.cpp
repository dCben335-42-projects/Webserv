#include "HttpSubRequest.hpp"
#include "Logger.hpp"
#include "httpExceptions.hpp"
#include "httpMimes.hpp"
#include "Date.hpp"

HttpSubRequest::HttpSubRequest(const std::string &rawStr)
{
	m_rawStr = String::trimBegin(rawStr);
	size_t i = 0;
	parse(m_rawStr, i);
	m_headers.remove(HEADER_CONTENT_TYPE);
}

HttpSubRequest::~HttpSubRequest()
{
}

const std::string &HttpSubRequest::getFileName() const
{
	return m_fileName;
}

const std::string &HttpSubRequest::getName() const
{
	return m_name;
}

std::string HttpSubRequest::extractValue(const std::string &token, const std::string &pattern) const
{
	if (token.size() <= pattern.size())
		throw Http400Exception();
	size_t			   firstQuote = token.find_first_of("\"");
	size_t			   lastQuote  = token.find_last_of("\"");
	const std::string &result	  = token.substr(firstQuote + 1, lastQuote - (firstQuote + 1));
	return (result);
}

void HttpSubRequest::parseNameAndFileName(Map<std::string> &headersMap)
{
	if (headersMap.get(HEADER_SUB_CONTENT_DISPOSITION).find("form-data;") == std::string::npos)
		throw Http400Exception();
	if (headersMap.get(HEADER_SUB_CONTENT_DISPOSITION).find("name=") == std::string::npos)
		throw Http400Exception();

	Vec<std::string> contentDispositionValues = String::split(headersMap.get(HEADER_SUB_CONTENT_DISPOSITION), ";");
	for (Vec<std::string>::const_iterator it = contentDispositionValues.begin(); it != contentDispositionValues.end(); it++)
	{
		std::string token = String::trim((*it));
		if (token == "form-data")
			continue;
		if (token.find("name=") == 0)
			m_name = extractValue(token, "name=");
		if (token.find("filename=") == 0)
			m_fileName = extractValue(token, "filename=");
		if (m_fileName.empty())
			m_fileName = Date::getCurrentDateStr();
	}
}

void HttpSubRequest::parseSubRequest()
{
	Map<std::string> headersMap = this->getHeaders();
	if (headersMap.size() > 2)
		throw Http400Exception();
	if (headersMap.has(HEADER_SUB_CONTENT_DISPOSITION) == false)
		throw Http400Exception();
	// if (headersMap.has(HEADER_CONTENT_TYPE) == false)
	// 	headersMap.set(HEADER_CONTENT_TYPE, MIME_PLAIN);
	parseNameAndFileName(headersMap);
}
