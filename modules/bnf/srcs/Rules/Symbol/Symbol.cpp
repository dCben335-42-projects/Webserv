#include "./Symbol.hpp"
#include "Logger.hpp"

Symbol::Symbol() : ARule(SYMBOL)
{
}

Symbol::Symbol(const std::string &val) : ARule(SYMBOL)
{
	value = val;
}

Symbol::~Symbol()
{
}

Vec<std::string> Symbol::getItems() const
{
	Vec<std::string> items;
	items.push_back(value);
	return items;
}


void Symbol::addItem(const std::string &val)
{
	value = val;
}

const std::string &Symbol::getValue() const
{
	return value;
}

std::string Symbol::getPrintableValue() const
{
	return getLogJson(value);
}

std::ostream &operator<<(std::ostream &os, const Symbol &data)
{
	os << data.getPrintableValue();
	return os;
}