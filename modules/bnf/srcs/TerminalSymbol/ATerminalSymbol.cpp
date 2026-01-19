#include "ATerminalSymbol.hpp"

ATerminalSymbol::~ATerminalSymbol()
{
}

bool ATerminalSymbol::operator==(const char &rhs) const
{
	if (!isTerminalChar())
		return false;
	return str[0] == rhs;
}

const std::string &ATerminalSymbol::getTerminalSymbol() const
{
	return str;
}

std::ostream &operator<<(std::ostream &os, const ATerminalSymbol &data)
{
	os << data.getTerminalSymbol();
	return os;
}