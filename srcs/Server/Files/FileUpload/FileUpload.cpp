#include "FileUpload.hpp"
#include "httpMimes.hpp"
#include "httpExceptions.hpp"
#include "unistd.h"
#include "FileManager.hpp"

FileUpload::FileUpload(HttpRequest &req) : m_req(req)
{
}

FileUpload::~FileUpload()
{
}

void FileUpload::fetchBoundaries(size_t boundaryPos, std::string contentType)
{
	m_boundaryStr	  = FILE_UPLOAD_BOUNDARY_MATCH_BEGIN_END + contentType.substr(boundaryPos + 9);
	m_lastBoundaryStr = m_boundaryStr + FILE_UPLOAD_BOUNDARY_MATCH_BEGIN_END;

	if (m_boundaryStr.size() < 3)
		throw Http400Exception(FILE_UPLOAD_400_INVALID_BOUNDARY_IN_HEADER);
}


bool FileUpload::isUploadValid()
{
	std::string contentType	   = m_req.getHeader(HEADER_CONTENT_TYPE);
	size_t		contentTypePos = contentType.find(MIME_MULTIPART);
	size_t		boundaryPos	   = contentType.find(FILE_UPLOAD_BOUNDARY_KEYWORD);

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
		throw Http400Exception(FILE_UPLOAD_400_NO_SUBTYPES);
	if (subTypes[(subTypes.size()) - 1] != FILE_UPLOAD_BOUNDARY_MATCH_BEGIN_END HEADER_LINE_ENDING)
		throw Http400Exception(FILE_UPLOAD_400_INVALID_FORM_DATA_ENDING);
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
		if (FileManager::exists(fullPath))
			throw Http409Exception();
		FileManager::createDiskFile(fullPath, it->getBody());
	}
}
