#ifndef FILEUPLOAD_HPP
#define FILEUPLOAD_HPP

#include "HttpRequest.hpp"
#include <string>

class FileUpload
{
	private:
		std::string				 m_boundaryStr;
		std::string				 m_lastBoundaryStr;
		HttpRequest				&m_req;
		Vec<AHttpInboundMessage> m_subRequests;

	public:
		FileUpload(HttpRequest &req);
		~FileUpload();
		bool isUploadValid();
		void fetchBoundaries(size_t boundaryPos, std::string contentType);
		void parseSubTypes();
		void fillSubRequests(Vec<std::string> subTypes);
		void parseSubRequests();
};

#endif