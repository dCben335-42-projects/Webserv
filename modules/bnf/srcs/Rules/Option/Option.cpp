#include "./Option.hpp"
#include "../../utils/utils.hpp"
using namespace bnf;

Option::Option(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr) : ARule(OPTION, conceptName, conceptDefIndex, ruleIndex, conceptPtr)
{
}
Option::~Option()
{
}

void Option::addItem(const TerminalSymbol &val)
{
	m_items.push_back(val);
}

Vec<TerminalSymbol> Option::getItems() const
{
	return m_items;
}

const std::string Option::getTypeName() const
{
	return "OPTION";
}

std::string Option::getPrintableValue() const
{
	return LoggerJson::get(m_items);
}