#include "./Symbol.hpp"
#include "Logger.hpp"
#include "../../utils/utils.hpp"
using namespace bnf;

Symbol::Symbol(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr) : ARule(SYMBOL, conceptName, conceptDefIndex, ruleIndex, conceptPtr)
{
}

Symbol::Symbol(const ATerminalSymbol &val, const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr) : ARule(SYMBOL, conceptName, conceptDefIndex, ruleIndex, conceptPtr)
{
	m_items.push_back(val);
}

Symbol::~Symbol()
{
}

Vec<ATerminalSymbol> Symbol::getItems() const
{
	return m_items;
}

void Symbol::addItem(const ATerminalSymbol &val)
{
	m_items.clear();
	m_items.push_back(val);
}

const std::string &Symbol::getValue() const
{
	if (m_items.empty())
		throw std::runtime_error("Symbol::getValue(): No items in symbol");
	return m_items[0].getValue();
}

std::string Symbol::getPrintableValue() const
{
	return LoggerJson::get(m_items[0].getValue());
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