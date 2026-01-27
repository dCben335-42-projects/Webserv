#include "./Sequence.hpp"
#include "../../utils/utils.hpp"
using namespace bnf;

Sequence::Sequence(
	const std::string &conceptName,
	size_t			   conceptDefIndex,
	size_t			   ruleIndex,
	Concept			  *conceptPtr,
	ConceptDef		  *conceptDefPtr) : ARule(SEQUENCE, conceptName, conceptDefIndex, ruleIndex, conceptPtr, conceptDefPtr)
{
}

Sequence::~Sequence()
{
}

void Sequence::addItem(const TerminalSymbol &val)
{
	m_items.push_back(val);
}

const Vec<TerminalSymbol> &Sequence::getItems() const
{
	return m_items;
}

const std::string Sequence::getTypeName() const
{
	return "SEQUENCE";
}

std::string Sequence::getPrintableValue() const
{
	return LoggerJson::get(m_items);
}