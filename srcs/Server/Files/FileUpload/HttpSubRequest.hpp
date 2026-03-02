#ifndef HTTPSUBREQUEST_HPP
#define HTTPSUBREQUEST_HPP

#include "AHttpInboundMessage.hpp"

#define ERROR_INVALID_CONTENT_DISP "Invalid Content-Disposition header: "
#define ERROR_INVALID_TOKEN "Invalid token: "
#define FORM_DATA_PARAM "form-data"
#define FORM_DATA_NAME "name"
#define FORM_DATA_FILENAME "filename"
#define ERROR_MISSING_FORM_DATA "missing 'form-data;' token"
#define ERROR_MISSING_NAME "missing 'name=' token"
#define ERROR_TOO_MANY_HEADER_SUBREQ "Too many headers in sub-request"
#define ERROR_MISSING_CONTENT_DISP_HEADER "Missing Content-Disposition header in sub-request"

class HttpSubRequest : public AHttpInboundMessage
{
	private:
		std::string m_fileName;
		std::string m_name;

		std::string extractValue(const std::string &token, const std::string &pattern) const;
		void		parseNameAndFileName();

	public:
		HttpSubRequest(const std::string &rawStr);
		~HttpSubRequest();

		void parseSubRequest();

		const std::string &getFileName() const;
		const std::string &getName() const;
};

#endif