#ifndef CGIOUTPUT_HPP
#define CGIOUTPUT_HPP

#include "AHttpInboundMessage.hpp"

class CgiOutput : public AHttpInboundMessage
{
	public:
		CgiOutput();
		~CgiOutput();
};

#endif