#ifndef TERMINAL_CHAR_HPP
#define TERMINAL_CHAR_HPP

#include <string>
#include "../ATerminalSymbol.hpp"

class TerminalChar : public ATerminalSymbol
{
	public:
		TerminalChar(const std::string &val);
		~TerminalChar();
};

std::ostream &operator<<(std::ostream &os, const TerminalChar &data);

#endif