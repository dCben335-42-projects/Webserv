#ifndef HTTPSUBREQUEST_HPP
#define HTTPSUBREQUEST_HPP

#include "AHttpInboundMessage.hpp"

class HttpSubRequest : public AHttpInboundMessage
{
	private:
		std::string m_fileName;
		std::string m_name;

		std::string extractValue(const std::string &token, const std::string &pattern) const;
		void		parseNameAndFileName(Map<std::string> &headersMap);
		std::string timestampGeneration();

	public:
		HttpSubRequest(const std::string &rawStr);
		~HttpSubRequest();

		void parseSubRequest();

		const std::string &getFileName() const;
		const std::string &getName() const;
};

#endif