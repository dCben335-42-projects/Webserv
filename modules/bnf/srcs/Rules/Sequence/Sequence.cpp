#include "./Sequence.hpp"
#include "Logger.hpp"
#include "../../utils/utils.hpp"

Sequence::Sequence(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr) : ARule(SEQUENCE, conceptName, conceptDefIndex, ruleIndex, conceptPtr)
{
}

Sequence::~Sequence()
{
}

void Sequence::addItem(const std::string &val)
{
	if (startsWith(val, "'"))
	{
		m_terminalChars.push_back(true);
	}
	else
	{
		m_terminalChars.push_back(false);
	}
	items.push_back(val);
}

Vec<std::string> Sequence::getItems() const
{
	return items;
}

const std::string Sequence::getTypeName() const
{
	return "SEQUENCE";
}

std::string Sequence::getPrintableValue() const
{
	return LoggerJson::get(items);
}