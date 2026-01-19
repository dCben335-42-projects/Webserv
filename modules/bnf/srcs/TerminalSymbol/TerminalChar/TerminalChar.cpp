#include "TerminalChar.hpp"


TerminalChar::TerminalChar(const std::string &val) : ATerminalSymbol(val, true)
{
}

TerminalChar::~TerminalChar()
{
}

std::ostream &operator<<(std::ostream &os, const TerminalChar &data)
{
	os << data.getValue();
	return os;
}