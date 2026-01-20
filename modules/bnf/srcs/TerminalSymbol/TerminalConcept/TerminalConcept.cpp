#include "TerminalConcept.hpp"

TerminalConcept::TerminalConcept()
{
}

TerminalConcept::~TerminalConcept()
{
}

bool TerminalConcept::isTerminalChar() const
{
	return false;
}

std::ostream &operator<<(std::ostream &os, const TerminalConcept &data)
{
	os << data.getTerminalSymbol();
	return os;
}