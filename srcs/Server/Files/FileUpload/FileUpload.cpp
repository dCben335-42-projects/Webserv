#include "FileUpload.hpp"
#include "config.hpp"
#include "httpExceptions.hpp"
#include "AHttpInboundMessage.hpp"
#include "HttpSubRequest.hpp"
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
	m_boundaryStr	  = "--" + contentType.substr(boundaryPos + 9);
	m_lastBoundaryStr = m_boundaryStr + "--";

	if (m_boundaryStr.size() < 3)
		throw Http400Exception();
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
	parseSubRequests();
	return true;
}

void FileUpload::parseSubTypes()
{
	const std::string &body			= m_req.getBody();
	Vec<std::string>   subTypes		= String::splitPattern(body, m_boundaryStr);
	size_t			   subTypesSize = subTypes.size();

	if (subTypesSize == 0)
		throw Http400Exception();
	if (subTypes[(subTypes.size()) - 1] != "--\r\n")
		throw Http400Exception();
	subTypes.remove(subTypesSize - 1);
	fillSubRequests(subTypes);
}

void FileUpload::fillSubRequests(const Vec<std::string> &subTypes)
{
	for (Vec<std::string>::const_iterator it = subTypes.begin(); it != subTypes.end(); it++)
	{
		HttpSubRequest subReq((*it));
		m_subRequests.push(subReq);
	}
}

void FileUpload::parseSubRequests()
{
	for (Vec<HttpSubRequest>::const_iterator it = m_subRequests.begin(); it != m_subRequests.end(); it++)
	{
		Map<std::string> headersMap = (*it).getHeaders();
		if (headersMap.size() > 2)
			throw Http400Exception();
		if (headersMap.has("Content-Disposition") == false)
			throw Http400Exception();
		if (headersMap.has(HEADER_CONTENT_TYPE) == false)
			headersMap.set(HEADER_CONTENT_TYPE, HEADER_TYPE_PLAIN);
		// parseNameAndFileName(headersMap);
	}
}

// void FileUpload::parseNameAndFileName(Map<std::string> &headersMap)
// {
// 	if (headersMap.get("Content-disposition").find("name=") == std::string::npos)
// 		throw Http400Exception();
// 	if (headersMap.get("Content-disposition").find("form-data") == std::string::npos)
// 		throw Http400Exception();
// 	// if (headersMap.get("Content-disposition").find("filename=") == std::string::npos)
// 	// TODO Generate filename with timestamp
// }

// // void FileUpload::createDiskFile()
// // {
// // }