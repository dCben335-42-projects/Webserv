#include "./Option.hpp"
#include "Logger.hpp"

Option::Option(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex) : ARule(OPTION, conceptName, conceptDefIndex, ruleIndex)
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

const std::string Option::getTypeName() const
{
	return "OPTION";
}

std::string Option::getPrintableValue() const
{
	log("Option Items:");
	return getLogJson(items);
}