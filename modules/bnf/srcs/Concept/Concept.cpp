#include "Concept.hpp"
#include <iostream>
#include <algorithm>
#include "../utils/utils.hpp"
#include "Logger.hpp"

using namespace bnf;

Concept::Concept()
{
}

void Concept::finish()
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

bool Concept::getIsTerminal() const
{
	return isTerminal;
}

Concept::~Concept()
{
	for (Vec<ARule *>::iterator it = values.begin(); it != values.end(); ++it)
	{
		delete (*it);
	}
}

void Concept::setRule(ARule *rule)
{
	values.push_back(rule);
}

Vec<ARule *> Concept::getRules()
{
	return values;
}

bool Concept::calledInConcept(std::string conceptName)
{
	for (Vec<ARule *>::iterator it = values.begin(); it != values.end(); ++it)
	{
		ARule			*rule  = *it;
		Vec<std::string> items = rule->getItems();
		if (rule->getType() != OPTION)
			continue;
		for (Vec<std::string>::iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
		{
			if (*itemIt == conceptName)
			{
				return true;
			}
		}
	}
	return false;
}

Vec<std::string> Concept::getItems()
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

	// std::cout << allItems.size() << " items found in concept." << std::endl;
	return allItems;
}

Vec<std::string> Concept::getItemsWithOption()
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


Vec<ARule *> Concept::getValues() const
{
	return values;
}

std::ostream &operator<<(std::ostream &os, const Concept &data)
{
	(void) os;
	// data.printRules();
	// (void)data;

	Vec<ARule *> values = data.getValues();

	log(values);

	// for (Vec<ARule *>::const_iterator it = values.begin(); it != values.end(); ++it) {
	// ARule* rule = *it;
	// os << *rule;
	// }

	return os;


	// os << "ConceptNode(name: " << data.name << ", concepts count: "
	//    << (data.concepts ? data.concepts->size() : 0) << ")";
	// return os;
}