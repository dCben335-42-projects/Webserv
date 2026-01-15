#include "./Group.hpp"
#include "Logger.hpp"
#include "../../utils/utils.hpp"

Group::Group(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr) : ARule(GROUP, conceptName, conceptDefIndex, ruleIndex, conceptPtr)
{
}
Group::~Group()
{
}

void Group::addItem(const std::string &val)
{
	if (startsWith(val, "'"))
	{
		m_terminalChars.push_back(true);
	}
	else
	{
		m_terminalChars.push_back(false);
	}
	items.push_back(val);
}

Vec<std::string> Group::getItems() const
{
	return items;
}

const std::string Group::getTypeName() const
{
	return "GROUP";
}

std::string Group::getPrintableValue() const
{
	return getLogJson(items);
}