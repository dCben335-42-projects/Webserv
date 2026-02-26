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
	deleteHeader(HEADER_CONTENT_TYPE);
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


/**
 *
 * @brief Extracts the value of the name="" or filename="" fields
 *
 * @throw Http400Exception() if the token to parse is too small compared to the fields
 * it means the request is invalid
 *
 */
std::string HttpSubRequest::extractValue(const std::string &token, const std::string &pattern) const
{
	if (token.size() <= pattern.size())
		throw Http400Exception(ERROR_INVALID_TOKEN + token);
	size_t			   firstQuote = token.find_first_of("\"");
	size_t			   lastQuote  = token.find_last_of("\"");
	const std::string &result	  = token.substr(firstQuote + 1, lastQuote - (firstQuote + 1));
	return (result);
}


/**
 *
 * @brief Checks if the form-data; and name= param is present in the subrequest
 * then extracts the values of each fields and store them in the class
 *
 * @throw Http400Exception() if the fields form-data or name= or not present
 *
 */
void HttpSubRequest::parseNameAndFileName()
{
	if (getHeader(HEADER_SUB_CONTENT_DISPOSITION).find(FORM_DATA_PARAM ";") == std::string::npos)
		throw Http400Exception(ERROR_INVALID_CONTENT_DISP ERROR_MISSING_FORM_DATA);
	if (getHeader(HEADER_SUB_CONTENT_DISPOSITION).find(FORM_DATA_NAME "=") == std::string::npos)
		throw Http400Exception(ERROR_INVALID_CONTENT_DISP ERROR_MISSING_NAME);

	Vec<std::string> contentDispositionValues = String::split(getHeader(HEADER_SUB_CONTENT_DISPOSITION), ";");
	for (Vec<std::string>::const_iterator it = contentDispositionValues.begin(); it != contentDispositionValues.end(); it++)
	{
		std::string token = String::trim((*it));
		if (token == FORM_DATA_PARAM)
			continue;
		if (token.find(FORM_DATA_NAME "=") == 0)
			m_name = extractValue(token, FORM_DATA_NAME "=");
		if (token.find(FORM_DATA_FILENAME "=") == 0)
			m_fileName = extractValue(token, FORM_DATA_FILENAME "=");
		if (m_fileName.empty())
			m_fileName = Date::getCurrentDateStr();
	}
}


/**
 *
 * @brief Parses a single subrequest, by checking the numbers of headers
 * and if the Content-Disposition Header is present
 *
 * @throw Http400Exception() if too many headers or if not Content-Disposition
 *
 */
void HttpSubRequest::parseSubRequest()
{
	const Map<std::string> &headersMap = getHeaders();
	if (headersMap.size() > 2)
		throw Http400Exception(ERROR_TOO_MANY_HEADER_SUBREQ);
	if (!hasHeader(HEADER_SUB_CONTENT_DISPOSITION))
		throw Http400Exception(ERROR_MISSING_CONTENT_DISP_HEADER);
	parseNameAndFileName();
}
