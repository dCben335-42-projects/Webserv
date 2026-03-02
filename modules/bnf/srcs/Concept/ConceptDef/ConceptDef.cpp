#include "ConceptDef.hpp"
#include <iostream>
#include <algorithm>
#include "../../utils/utils.hpp"

using namespace bnf;

/**
 * @brief Constructs a ConceptDef object. It initializes the m_rules member variable as an empty ARulePath.
 */
ConceptDef::ConceptDef()
{
}

/**
 * @brief Destructor for the ConceptDef class. It deletes all the ARule objects stored in the ConceptDef.
 */
ConceptDef::~ConceptDef()
{
	size_t size = m_rules.size();
	for (size_t i = 0; i < size; ++i)
		delete m_rules[i];
}

/**
 * @brief Retrieves the number of ARules contained in this ConceptDef.
 * @return The number of ARules in the ConceptDef.
 */
size_t ConceptDef::size() const
{
	return m_rules.size();
}


/**
 * @brief Adds a new ARule to the ConceptDef. If there are already existing rules, the new rule is linked as the next rule of the last rule.
 * @param rule A pointer to the ARule to be added to the ConceptDef.
 */
void ConceptDef::setRule(const ARule *rule)
{
	if (m_rules.size() > 0)
	{
		ARule *lastRule = const_cast<ARule *>(m_rules.back());
		lastRule->setNextRules(rule);
	}
	m_rules.push_back(rule);
}

/**
 * @brief Retrieves the ARulePath containing all the rules of this ConceptDef.
 * @return A constant reference to the ARulePath containing all the rules.
 */
const ARulePath &ConceptDef::getRules() const
{
	return m_rules;
}

/**
 * @brief Retrieves the ARule at the specified index without bounds checking.
 * @param index The index of the ARule to retrieve.
 * @return A pointer to the ARule at the specified index.
 */
const ARule *ConceptDef::getARuleUnsafe(const size_t index) const
{
	return m_rules[index];
}

/**
 * @brief Retrieves a vector of TerminalSymbols representing all the items in the rules of this ConceptDef, excluding those from OPTION rules.
 * @return A vector of TerminalSymbols containing all the items from non-OPTION rules.
 */
const Vec<TerminalSymbol> ConceptDef::getItems() const
{
	Vec<TerminalSymbol> allItems;
	const size_t		i_max = m_rules.size();
	for (size_t i = 0; i < i_max; ++i)
	{
		const ARule *const rule = m_rules[i];
		if (rule->is(OPTION))
			continue;
		const Vec<TerminalSymbol> &items = rule->getItems();

		const size_t j_max = items.size();
		for (size_t j = 0; j < j_max; ++j)
			if (std::find(allItems.begin(), allItems.end(), items[j]) == allItems.end())
				allItems.push_back(items[j]);
	}
	return allItems;
}

/**
 * @brief Retrieves a vector of TerminalSymbols representing all the items in the rules of this ConceptDef, including those from OPTION rules.
 * @return A vector of TerminalSymbols containing all the items from all rules.
 */
const Vec<TerminalSymbol> ConceptDef::getItemsValueWithOption() const
{
	Vec<TerminalSymbol> allItems;
	const size_t		i_max = m_rules.size();
	for (size_t i = 0; i < i_max; ++i)
	{
		const ARule *const		   rule	 = m_rules[i];
		const Vec<TerminalSymbol> &items = rule->getItems();

		const size_t j_max = items.size();
		for (size_t j = 0; j < j_max; ++j)
			if (std::find(allItems.begin(), allItems.end(), items[j]) == allItems.end())
				allItems.push_back(items[j]);
	}
	return allItems;
}

/**
 * @brief Overloads the stream insertion operator to output the ConceptDef in a JSON-like format using LoggerJson.
 * @param os The output stream to which the ConceptDef will be written.
 * @param data The ConceptDef object to be outputted.
 * @return A reference to the output stream after writing the ConceptDef.
 */
std::ostream &operator<<(std::ostream &os, const ConceptDef &data)
{
	os << LoggerJson::get(data.getRules());
	return os;
}