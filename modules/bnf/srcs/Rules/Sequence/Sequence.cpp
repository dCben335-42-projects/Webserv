#include "./Sequence.hpp"
#include "Logger.hpp"

Sequence::Sequence() : ARule(SEQUENCE)
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

std::string Sequence::getPrintableValue() const
{
	return getLogJson(items);
}