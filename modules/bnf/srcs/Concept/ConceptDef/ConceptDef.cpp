#include "ConceptDef.hpp"
#include <iostream>
#include <algorithm>
#include "../../utils/utils.hpp"

using namespace bnf;

ConceptDef::ConceptDef(const std::string conceptName) : m_conceptName(conceptName), isTerminal(false)
{
}

size_t ConceptDef::size() const
{
	return values.size();
}

void ConceptDef::finish()
{
	Vec<std::string> items = getItemsWithOption();
	for (Vec<std::string>::iterator it = items.begin(); it != items.end(); ++it)
	{
		if (!startsWith(*it, "\'"))
		{
			isTerminal = false;
			return;
		}
	}
	isTerminal = true;
}

bool ConceptDef::getIsTerminal() const
{
	return isTerminal;
}

ConceptDef::~ConceptDef()
{
	for (ARulePath::iterator it = values.begin(); it != values.end(); ++it)
	{
		delete (*it);
	}
}

void ConceptDef::setRule(const ARule *rule)
{
	if (values.size() > 0)
	{
		ARule *lastRule = const_cast<ARule *>(values.back());
		lastRule->setNextRules(rule);
	}
	values.push_back(rule);
}

ARulePath ConceptDef::getRules()
{
	return values;
}

Vec<TerminalSymbol> ConceptDef::getItems()
{
	Vec<TerminalSymbol> allItems;
	for (ARulePath::iterator it = values.begin(); it != values.end(); ++it)
	{
		const ARule *rule = *it;
		if (rule->getType() == OPTION)
			continue;
		Vec<TerminalSymbol> items = rule->getItems();

		for (size_t i = 0; i < items.size(); ++i)
		{
			if (std::find(allItems.begin(), allItems.end(), items[i]) == allItems.end())
				allItems.push_back(items[i]);
		}
	}

	return allItems;
}

Vec<std::string> ConceptDef::getItemsValue()
{
	Vec<std::string> allItems;
	for (ARulePath::iterator it = values.begin(); it != values.end(); ++it)
	{
		const ARule *rule = *it;
		if (rule->getType() == OPTION)
			continue;
		Vec<TerminalSymbol> items = rule->getItems();

		for (size_t i = 0; i < items.size(); ++i)
		{
			if (std::find(allItems.begin(), allItems.end(), items[i].getValue()) == allItems.end())
				allItems.push_back(items[i].getValue());
		}
	}

	return allItems;
}

Vec<std::string> ConceptDef::getItemsWithOption()
{
	Vec<std::string> allItems;
	for (ARulePath::iterator it = values.begin(); it != values.end(); ++it)
	{
		const ARule		   *rule  = *it;
		Vec<TerminalSymbol> items = rule->getItems();

		for (size_t i = 0; i < items.size(); ++i)
		{
			if (std::find(allItems.begin(), allItems.end(), items[i].getValue()) == allItems.end())
				allItems.push_back(items[i].getValue());
		}
	}
	return allItems;
}


ARulePath ConceptDef::getValues() const
{
	return values;
}

const ARule *ConceptDef::getARule(const size_t index) const
{
	if (values.size() > index)
		return values[index];
	return NULL;
}

Vec<TerminalSymbol> ConceptDef::getItemsOfARule(size_t index)
{
	if (values.size() > index)
		return values[index]->getItems();
	throw std::runtime_error("Index out of range in getItemsOfARule");
}


void ConceptDef::computeOptionalIndex()
{
	size_t size		 = values.size();
	optionalEndIndex = size;
	for (int i = values.size() - 1; i >= 0; --i)
	{
		const ARule *rule = values[i];
		if (rule->getType() == OPTION)
			optionalEndIndex = i;
		else
			break;
	}
}

size_t ConceptDef::getOptionalEndIndex() const
{
	return optionalEndIndex;
}

std::ostream &operator<<(std::ostream &os, const ConceptDef &data)
{
	os << LoggerJson::get(data.getValues());
	return os;
}