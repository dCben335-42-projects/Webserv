#include "./Symbol.hpp"
#include "Logger.hpp"
#include "../../utils/utils.hpp"

Symbol::Symbol(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr) : ARule(SYMBOL, conceptName, conceptDefIndex, ruleIndex, conceptPtr)
{
}

Symbol::Symbol(const std::string &val, const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr) : ARule(SYMBOL, conceptName, conceptDefIndex, ruleIndex, conceptPtr)
{
	value = val;
}

Symbol::~Symbol()
{
}

Vec<std::string> Symbol::getItems() const
{
	Vec<std::string> items;
	items.push_back(value);
	return items;
}


void Symbol::addItem(const std::string &val)
{
	if (startsWith(val, "'"))
	{
		m_terminalChars.push_back(true);
	}
	else
	{
		m_terminalChars.push_back(false);
	}
	value = val;
}

const std::string &Symbol::getValue() const
{
	return value;
}

std::string Symbol::getPrintableValue() const
{
	return getLogJson(value);
}

const std::string Symbol::getTypeName() const
{
	return "SYMBOL";
}

std::ostream &operator<<(std::ostream &os, const Symbol &data)
{
	os << data.getPrintableValue();
	return os;
}