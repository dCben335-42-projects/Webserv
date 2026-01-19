#include "ATerminalSymbol.hpp"

ATerminalSymbol::ATerminalSymbol(const std::string &val, bool isTerminalChar) : m_str(val), m_isTerminalChar(isTerminalChar)
{
}

ATerminalSymbol::~ATerminalSymbol()
{
}

bool ATerminalSymbol::isTerminalChar() const
{
	return m_isTerminalChar;
}

bool ATerminalSymbol::operator==(const char &rhs) const
{
	if (!m_isTerminalChar)
		return false;
	return m_str[0] == rhs;
}

bool ATerminalSymbol::operator==(const ATerminalSymbol &rhs) const
{
	return m_str == rhs.m_str && m_isTerminalChar == rhs.m_isTerminalChar;
}


const std::string &ATerminalSymbol::getValue() const
{
	return m_str;
}

ATerminalSymbol ATerminalSymbol::specify(const std::string &val)
{
	if (val.length() >= 2 && (val[0] == '\'' && val[val.length() - 1] == '\''))
		return ATerminalSymbol(val.substr(1, val.length() - 2), true);
	return ATerminalSymbol(val, false);
}

std::ostream &operator<<(std::ostream &os, const ATerminalSymbol &data)
{
	os << data.getValue();
	return os;
}