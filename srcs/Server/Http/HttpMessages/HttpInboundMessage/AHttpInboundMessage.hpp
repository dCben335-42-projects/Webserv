#ifndef HTTP_INBOUND_MESSAGE_HPP
#define HTTP_INBOUND_MESSAGE_HPP

#include "AHttpMessage.hpp"

class AHttpInboundMessage : public AHttpMessage
{
	protected:
		std::string m_rawStr;
		std::string m_target;

		size_t				 parseHeaderLine(const std::string &rawRequest, const size_t pos);
		AHttpInboundMessage &parse(std::string &rawRequest, size_t &pos);


	public:
		AHttpInboundMessage();
		~AHttpInboundMessage();

		std::string getRawRequest() const;
		std::string getTarget() const;

		void setRawRequest(const std::string &rawRequest);
};

#endif
