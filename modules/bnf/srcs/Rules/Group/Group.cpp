#include "./Group.hpp"
#include "Logger.hpp"

Group::Group(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex) : ARule(GROUP, conceptName, conceptDefIndex, ruleIndex)
{
}
Group::~Group()
{
}

void Group::addItem(const std::string &val)
{
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