#ifndef FILEUPLOAD_HPP
#define FILEUPLOAD_HPP

#include "HttpRequest.hpp"
#include <string>

class FileUpload
{
	private:
		std::string	 m_boundaryStr;
		std::string	 m_lastBoundaryStr;
		HttpRequest &m_req;

	public:
		FileUpload(HttpRequest &req);
		~FileUpload();
		bool isUploadValid();
		bool isSubTypeValid();
		void fetchBoundaries(size_t boundaryPos, std::string contentType);
};

#endif