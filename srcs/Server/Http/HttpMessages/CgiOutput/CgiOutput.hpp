#ifndef CGIOUTPUT_HPP
#define CGIOUTPUT_HPP

#include "AHttpInboundMessage.hpp"

class CgiOutput : public AHttpInboundMessage
{
	private:
		std::string m_res;

		bool parseFirstOutputLine(const size_t endOfLineIndex);
		void completeOutput(bool keepAlive);

	public:
		void			   parse(bool keepAlive);
		void			   appendToCgiOutput(const char *rawStr, size_t size);
		void			   build();
		const std::string &getRes() const;
		CgiOutput();
		~CgiOutput();
};

#endif