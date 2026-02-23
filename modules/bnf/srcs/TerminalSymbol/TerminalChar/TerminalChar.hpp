#ifndef BNF_TERMINAL_CHAR_HPP
#define BNF_TERMINAL_CHAR_HPP

#include <string>
#include "../ATerminalSymbol.hpp"

class TerminalChar : public ATerminalSymbol
{
	public:
		TerminalChar();
		~TerminalChar();

		bool isTerminalChar() const;
};

std::ostream &operator<<(std::ostream &os, const TerminalChar &data);

#endif