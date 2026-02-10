#include "CgiOutput.hpp"
#include "httpMimes.hpp"
#include "Number.hpp"
#include "Vec.hpp"
#include "String.hpp"
#include "webserv.hpp"
#include "Map.hpp"

CgiOutput::CgiOutput()
{
}

CgiOutput::~CgiOutput()
{
}

void CgiOutput::build()
{
	m_res = AHttpMessage::build();
}

const std::string &CgiOutput::getRes() const
{
	return m_res;
}

void CgiOutput::parse(bool keepAlive)
{
	size_t headerEndPos = m_rawStr.find(HEADER_ENDING);
	if (headerEndPos == std::string::npos)
	{
		m_statusCode = (m_rawStr.size() == 0 ? 204 : 200);
		m_body		 = m_rawStr;
		completeOutput(keepAlive);
		return;
	}
	m_body				  = m_rawStr.substr(headerEndPos + HEADER_ENDING_SIZE);
	size_t endOfLineIndex = m_rawStr.find(HEADER_LINE_ENDING);
	if (!parseFirstOutputLine(endOfLineIndex))
		m_statusCode = 200;
	endOfLineIndex += HEADER_LINE_ENDING_SIZE;
	try
	{
		AHttpInboundMessage::parse(m_rawStr, endOfLineIndex);
	}
	catch (std::invalid_argument &e)
	{
	}
	completeOutput(keepAlive);
}

void CgiOutput::appendToCgiOutput(const char *rawStr, size_t size)
{
	m_rawStr.append(rawStr, size);
}

bool CgiOutput::parseFirstOutputLine(const size_t endOfLineIndex)
{
	std::string		 firstLine		   = m_rawStr.substr(0, endOfLineIndex);
	Vec<std::string> firstLineSettings = String::split(m_rawStr, " ");
	if (firstLineSettings.size() != 3)
		return false;
	if (firstLineSettings[0] != PROTOCOL)
		return false;
	m_statusCode = Number::fromString(firstLineSettings[1]);
	if (!m_statusReasonMap.has(m_statusCode))
		return false;
	return true;
}

void CgiOutput::completeOutput(bool keepAlive)
{
	if (!hasHeader(HEADER_CONTENT_TYPE))
		setHeader(HEADER_CONTENT_TYPE, MIME_HTML);
	if (!hasHeader(HEADER_CONTENT_LENGTH))
		setHeader(HEADER_CONTENT_LENGTH, Number::toString(m_body.size()));
	setHeader(HEADER_CONNECTION, keepAlive ? HEADER_CONNECTION_KEEP_ALIVE : HEADER_CONNECTION_CLOSE);
}
