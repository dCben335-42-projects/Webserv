#include "./Group.hpp"
#include "Logger.hpp"

Group::Group() : ARule(GROUP)
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

std::string Group::getPrintableValue() const
{
	return LoggerJson::get(items);
}