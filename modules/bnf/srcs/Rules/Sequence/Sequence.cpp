#include "./Sequence.hpp"
#include "Logger.hpp"
#include "../../utils/utils.hpp"
using namespace bnf;

Sequence::Sequence(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr) : ARule(SEQUENCE, conceptName, conceptDefIndex, ruleIndex, conceptPtr)
{
}

Sequence::~Sequence()
{
}

void Sequence::addItem(const TerminalSymbol &val)
{
	m_items.push_back(val);
}

Vec<TerminalSymbol> Sequence::getItems() const
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