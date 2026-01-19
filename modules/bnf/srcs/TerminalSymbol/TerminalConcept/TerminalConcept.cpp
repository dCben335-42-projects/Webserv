#include "TerminalConcept.hpp"

TerminalConcept::TerminalConcept(const std::string &val) : ATerminalSymbol(val, false)
{
}

TerminalConcept::~TerminalConcept()
{
}

std::ostream &operator<<(std::ostream &os, const TerminalConcept &data)
{
	os << data.getValue();
	return os;
}