#include "./Option.hpp"
#include "../../utils/utils.hpp"
using namespace bnf;

Option::Option(
	const std::string &conceptName,
	size_t			   conceptDefIndex,
	size_t			   ruleIndex,
	Concept			  *conceptPtr,
	ConceptDef		  *conceptDefPtr) : ARule(OPTION, conceptName, conceptDefIndex, ruleIndex, conceptPtr, conceptDefPtr)
{
}
Option::~Option()
{
}

void Option::addItem(const TerminalSymbol &val)
{
	m_items.push_back(val);
	if (val.isTerminalChar())
		m_itemsValue.push_back(val.getValue());
}

const Vec<TerminalSymbol> &Option::getItems() const
{
	return m_items;
}

const Vec<std::string> &Option::getItemsValue() const
{
	return m_itemsValue;
}

const std::string Option::getTypeName() const
{
	return "OPTION";
}

std::string Option::getPrintableValue() const
{
	return LoggerJson::get(m_items);
}