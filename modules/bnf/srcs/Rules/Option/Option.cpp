#include "./Option.hpp"
#include "Logger.hpp"

Option::Option() : ARule(OPTION)
{
}
Option::~Option()
{
}

void Option::addItem(const std::string &val)
{
	items.push_back(val);
}

Vec<std::string> Option::getItems() const
{
	return items;
}



std::string Option::getPrintableValue() const
{
	log("Option Items:");
	return getLogJson(items);
}