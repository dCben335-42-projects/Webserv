#include "TerminalChar.hpp"


TerminalChar::TerminalChar()
{
}

TerminalChar::~TerminalChar()
{
}

bool TerminalChar::isTerminalChar() const
{
	return true;
}

std::ostream &operator<<(std::ostream &os, const TerminalChar &data)
{
	os << data.getTerminalSymbol();
	return os;
}