#include "./Sequence.hpp"
#include "Logger.hpp"

Sequence::Sequence(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex) : ARule(SEQUENCE, conceptName, conceptDefIndex, ruleIndex)
{
}

Sequence::~Sequence()
{
}

void Sequence::addItem(const std::string &val)
{
	items.push_back(val);
}

Vec<std::string> Sequence::getItems() const
{
	return items;
}

const std::string Sequence::getTypeName() const
{
	return "SEQUENCE";
}

std::string Sequence::getPrintableValue() const
{
	log("Sequence Items:");
	return getLogJson(items);
}