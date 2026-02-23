#include "TerminalSymbol.hpp"
#include "bnf_defs.hpp"

TerminalSymbol::TerminalSymbol(const std::string &val, bool isTerminalChar) : m_str(val), m_isTerminalChar(isTerminalChar)
{
}

TerminalSymbol::~TerminalSymbol()
{
}

bool TerminalSymbol::isTerminalChar() const
{
	return m_isTerminalChar;
}

bool TerminalSymbol::operator==(const char &rhs) const
{
	if (!m_isTerminalChar)
		return false;
	return m_str[0] == rhs;
}

bool TerminalSymbol::operator==(const TerminalSymbol &rhs) const
{
	return m_str == rhs.m_str && m_isTerminalChar == rhs.m_isTerminalChar;
}

const std::string &TerminalSymbol::getValue() const
{
	return m_str;
}

const std::string *TerminalSymbol::getValuePtr() const
{
	return &m_str;
}

const char *TerminalSymbol::getCharOfValue(size_t index) const
{
	return &m_str[index];
}

TerminalSymbol TerminalSymbol::specify(const std::string &val)
{
	if (val.length() >= 2 && (val[0] == '\'' && val[val.length() - 1] == '\''))
		return TerminalSymbol(val.substr(1, val.length() - 2), true);
	return TerminalSymbol(val, false);
}

std::ostream &operator<<(std::ostream &os, const TerminalSymbol &data)
{
	os << "{";
	os << "my_address: " << &data << ", ";
	os << "value: " << data.getValue();
	os << "}";
	return os;
}