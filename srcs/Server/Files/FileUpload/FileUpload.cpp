#include "FileUpload.hpp"
#include "config.hpp"
#include "httpExceptions.hpp"
#include "AHttpInboundMessage.hpp"
// TODO
#include "Logger.hpp"

FileUpload::FileUpload(HttpRequest &req) : m_req(req)
{
}

FileUpload::~FileUpload()
{
}

void FileUpload::fetchBoundaries(size_t boundaryPos, std::string contentType)
{
	m_boundaryStr	  = contentType.substr(boundaryPos + 9);
	m_lastBoundaryStr = m_boundaryStr + "--";

	if (m_boundaryStr.size() < 2 || m_boundaryStr.find("--" != 0))
		throw Http400Exception();
	Logger::log("Boundary str: " + m_boundaryStr);
	Logger::log("Last boundary str: " + m_lastBoundaryStr);
}


bool FileUpload::isUploadValid()
{
	std::string contentType	   = m_req.getHeader(HEADER_CONTENT_TYPE);
	size_t		contentTypePos = contentType.find(HEADER_TYPE_MULTIPART);
	size_t		boundaryPos	   = contentType.find("boundary=");

	if (contentTypePos == std::string::npos || boundaryPos == std::string::npos)
		return false;
	fetchBoundaries(boundaryPos, contentType);
	parseSubTypes();
	return true;
}

void FileUpload::parseSubTypes()
{
	const std::string &body			= m_req.getBody();
	Vec<std::string>   subTypes		= String::splitPattern(body, m_boundaryStr);
	size_t			   subTypesSize = subTypes.size();

	if (subTypesSize == 0)
		throw Http400Exception();
	if (subTypes[subTypes.size() - 1] != "--")
		throw Http400Exception();
	fillSubRequests(subTypes);
}


void FileUpload::fillSubRequests(Vec<std::string> subTypes)
{
	for (Vec<std::string>::const_iterator it = subTypes.begin(); it != subTypes.end(); it++)
	{
		AHttpInboundMessage subReq;
		subReq.setRawRequest((*it));
		m_subRequests.push(subReq);
	}
}

// void FileUpload::parseSubRequests()
// {
// 	for (Vec<AHttpInboundMessage>::const_iterator it = m_subRequests.begin(); it != m_subRequests.end(); it++)
// 	{
// 	}
// }