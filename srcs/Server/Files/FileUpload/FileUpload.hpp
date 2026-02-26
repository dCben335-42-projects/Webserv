#ifndef FILEUPLOAD_HPP
#define FILEUPLOAD_HPP

#include "HttpRequest.hpp"
#include "HttpSubRequest.hpp"
#include <string>

#define FILE_UPLOAD_BOUNDARY_KEYWORD "boundary="
#define FILE_UPLOAD_400_NO_SUBTYPES "No subtypes found in multipart/form-data body"
#define FILE_UPLOAD_400_INVALID_FORM_DATA_ENDING "Invalid multipart/form-data ending"
#define FILE_UPLOAD_400_INVALID_BOUNDARY_IN_HEADER "Invalid boundary in Content-Type header"
#define FILE_UPLOAD_BOUNDARY_MATCH_BEGIN_END "--"

class FileUpload
{
	private:
		std::string			m_boundaryStr;
		std::string			m_lastBoundaryStr;
		HttpRequest		   &m_req;
		Vec<HttpSubRequest> m_subRequests;

		void fetchBoundaries(size_t boundaryPos, std::string contentType);
		void parseSubTypes();
		void fillSubRequests(const Vec<std::string> &subTypes);

	public:
		FileUpload(HttpRequest &req);
		~FileUpload();

		void parseAndUpload();
		bool isUploadValid();
};

#endif