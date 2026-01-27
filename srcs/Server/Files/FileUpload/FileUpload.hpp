#ifndef FILEUPLOAD_HPP
#define FILEUPLOAD_HPP

#include "HttpRequest.hpp"
#include "HttpSubRequest.hpp"
#include <string>

class FileUpload
{
	private:
		std::string			m_boundaryStr;
		std::string			m_lastBoundaryStr;
		HttpRequest		   &m_req;
		Vec<HttpSubRequest> m_subRequests;

	public:
		FileUpload(HttpRequest &req);
		~FileUpload();
		bool isUploadValid();
		void fetchBoundaries(size_t boundaryPos, std::string contentType);
		void parseSubTypes();
		void fillSubRequests(const Vec<std::string> &subTypes);
		void parseSubRequests();
		void createDiskFile();
		void parseNameAndFileName(Map<std::string> &headersMap);
};

#endif