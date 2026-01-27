#ifndef HTTPSUBREQUEST_HPP
#define HTTPSUBREQUEST_HPP

#include "AHttpInboundMessage.hpp"

class HttpSubRequest : public AHttpInboundMessage
{
	private:
		std::string m_fileName;
		std::string m_name;

	public:
		HttpSubRequest(const std::string &rawStr);
		~HttpSubRequest();
};

#endif