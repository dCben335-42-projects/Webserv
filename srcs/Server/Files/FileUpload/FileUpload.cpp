#include "FileUpload.hpp"
#include "httpMimes.hpp"
#include "httpExceptions.hpp"
#include "AHttpInboundMessage.hpp"
#include "HttpSubRequest.hpp"
#include "fcntl.h"
#include "unistd.h"
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
	size_t		contentTypePos = contentType.find(MIME_MULTIPART);
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
	const std::string &uploadPath = m_req.getUploadPath();
	for (Vec<HttpSubRequest>::iterator it = m_subRequests.begin(); it != m_subRequests.end(); it++)
	{
		(*it).parseSubRequest();
		const std::string &fullPath = String::removeDuplicateSlashes(uploadPath + "/" + it->getFileName());
		if (access(fullPath.c_str(), F_OK) == 0)
			throw Http409Exception();
		createDiskFile(fullPath, it->getBody());
	}
}

void FileUpload::createDiskFile(const std::string &fullPath, const std::string &body)
{
	std::ofstream file;
	file.open(fullPath.c_str(), std::ofstream::trunc);
	if (!file.is_open())
		throw Http500Exception();
	file << body;
	file.close();
}
