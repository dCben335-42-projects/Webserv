#include "./Group.hpp"
#include "Logger.hpp"
#include "../../utils/utils.hpp"
using namespace bnf;

Group::Group(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr) : ARule(GROUP, conceptName, conceptDefIndex, ruleIndex, conceptPtr)
{
}
Group::~Group()
{
}

void Group::addItem(const ATerminalSymbol &val)
{
	m_items.push_back(val);
}

Vec<ATerminalSymbol> Group::getItems() const
{
	return m_items;
}

const std::string Group::getTypeName() const
{
	return "GROUP";
}

std::string Group::getPrintableValue() const
{
	return LoggerJson::get(m_items);
}