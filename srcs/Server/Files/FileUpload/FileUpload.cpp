#include "FileUpload.hpp"
#include "config.hpp"
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
	return true;
}
