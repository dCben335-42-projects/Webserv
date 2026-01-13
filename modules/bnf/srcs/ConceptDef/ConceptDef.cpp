#include "ConceptDef.hpp"
#include <iostream>
#include <algorithm>
#include "../utils/utils.hpp"
#include "Logger.hpp"

using namespace bnf;

ConceptDef::ConceptDef(const std::string conceptName) : m_conceptName(conceptName), isTerminal(false)
{
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
	for (Vec<ARule *>::iterator it = values.begin(); it != values.end(); ++it)
	{
		delete (*it);
	}
}

void ConceptDef::setRule(ARule *rule)
{
	values.push_back(rule);
}

Vec<ARule *> ConceptDef::getRules()
{
	return values;
}

Vec<std::string> ConceptDef::getItems()
{
	Vec<std::string> allItems;
	for (Vec<ARule *>::iterator it = values.begin(); it != values.end(); ++it)
	{
		ARule *rule = *it;
		if (rule->getType() == OPTION)
			continue;
		Vec<std::string> items = rule->getItems();

		for (size_t i = 0; i < items.size(); ++i)
		{
			if (std::find(allItems.begin(), allItems.end(), items[i]) == allItems.end())
				allItems.push_back(items[i]);
		}
	}

	return allItems;
}

Vec<std::string> ConceptDef::getItemsWithOption()
{
	Vec<std::string> allItems;
	for (Vec<ARule *>::iterator it = values.begin(); it != values.end(); ++it)
	{
		ARule			*rule  = *it;
		Vec<std::string> items = rule->getItems();

		for (size_t i = 0; i < items.size(); ++i)
		{
			if (std::find(allItems.begin(), allItems.end(), items[i]) == allItems.end())
				allItems.push_back(items[i]);
		}
	}
	return allItems;
}


Vec<ARule *> ConceptDef::getValues() const
{
	return values;
}

void ConceptDef::addParent(ARule *parent)
{
	m_parents.insert(parent);
}

std::ostream &operator<<(std::ostream &os, const ConceptDef &data)
{
	(void) os;
	Vec<ARule *> values = data.getValues();
	log(values);
	return os;
}