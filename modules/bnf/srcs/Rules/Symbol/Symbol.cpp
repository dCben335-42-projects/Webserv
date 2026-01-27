#include "./Symbol.hpp"
#include "../../utils/utils.hpp"

using namespace bnf;

Symbol::Symbol(
	const std::string &conceptName,
	size_t			   conceptDefIndex,
	size_t			   ruleIndex,
	Concept			  *conceptPtr,
	ConceptDef		  *conceptDefPtr) : ARule(SYMBOL, conceptName, conceptDefIndex, ruleIndex, conceptPtr, conceptDefPtr)
{
}

Symbol::Symbol(
	const TerminalSymbol &val,
	const std::string	 &conceptName,
	size_t				  conceptDefIndex,
	size_t				  ruleIndex,
	Concept				 *conceptPtr,
	ConceptDef			 *conceptDefPtr) : ARule(SYMBOL, conceptName, conceptDefIndex, ruleIndex, conceptPtr, conceptDefPtr)
{
	m_items.push_back(val);
}

Symbol::~Symbol()
{
}

const Vec<TerminalSymbol> &Symbol::getItems() const
{
	return m_items;
}

void Symbol::addItem(const TerminalSymbol &val)
{
	m_items.clear();
	m_items.push_back(val);
}

const std::string &Symbol::getValue() const
{
	if (m_items.empty())
		throw std::runtime_error("Symbol::getValue(): No items in symbol");
	return m_items[0].getValue();
}

std::string Symbol::getPrintableValue() const
{
	return LoggerJson::get(m_items[0].getValue());
}

const std::string Symbol::getTypeName() const
{
	return "SYMBOL";
}

std::ostream &operator<<(std::ostream &os, const Symbol &data)
{
	os << data.getPrintableValue();
	return os;
}