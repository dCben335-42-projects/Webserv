#include "ARule.hpp"

#include "./Group/Group.hpp"
#include "./Sequence/Sequence.hpp"
#include "./Option/Option.hpp"
#include "./Symbol/Symbol.hpp"

#include "Logger.hpp"

using namespace bnf;

ARule::ARule(ERuleType t, const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex) : m_type(t), m_conceptName(conceptName), m_conceptDefIndex(conceptDefIndex), m_ruleIndex(ruleIndex), m_nextRule(NULL)
{
}

ARule::~ARule()
{
}

ARule &ARule::operator=(const ARule &other)
{
	if (this == &other)
		return *this;

	return *this;
}

ERuleType ARule::getType() const
{
	return m_type;
}

ARule *createGroup(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex)
{
	return new Group(conceptName, conceptDefIndex, ruleIndex);
}

ARule *createSequence(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex)
{
	return new Sequence(conceptName, conceptDefIndex, ruleIndex);
}

ARule *createOption(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex)
{
	return new Option(conceptName, conceptDefIndex, ruleIndex);
}

ARule *createSymbol(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex)
{
	return new Symbol(conceptName, conceptDefIndex, ruleIndex);
}

static const BnfMap rules[] = {
	{.opener	  = LBRACE,
	 .closer	  = RBRACE,
	 .constructor = &createGroup,
	 .hasClosure  = true},
	{.opener	  = LBRACKET,
	 .closer	  = RBRACKET,
	 .constructor = &createSequence,
	 .hasClosure  = true},
	{.opener	  = LHOOK,
	 .closer	  = RHOOK,
	 .constructor = &createOption,
	 .hasClosure  = true},
	{.opener	  = "",
	 .closer	  = "",
	 .constructor = &createSymbol,
	 .hasClosure  = false},
};

bool ARule::isKeyWord(const std::string &word, bool onlyClose)
{
	size_t i = 0;

	while (rules[i].hasClosure)
	{
		if (!onlyClose && word == rules[i].opener)
			return true;
		if (word == rules[i].closer)
			return true;
		i++;
	}
	return false;
}

BnfMap ARule::getMap(const std::string &opener)
{
	size_t i = 0;

	while (rules[i].hasClosure)
	{
		if (opener == rules[i].opener)
			return rules[i];
		i++;
	}
	return rules[i];
}

const std::string ARule::getConceptName() const
{
	return m_conceptName;
}

size_t ARule::getConceptDefIndex() const
{
	return m_conceptDefIndex;
}

size_t ARule::getRuleIndex() const
{
	return m_ruleIndex;
}

const ARule *ARule::getNextRule() const
{
	return m_nextRule;
}

void ARule::setNextRule(ARule *nextRule)
{
	m_nextRule = nextRule;
}

std::ostream &operator<<(std::ostream &os, const ARule &data)
{
	os << Log::indent() << "{\n";
	Log::incrementIndent();
	os << Log::indent() << "position: " << data.getConceptName() << "_" << data.getConceptDefIndex() << ",\n"
	   << Log::indent() << "type: " << data.getTypeName() << ",\n"
	   << Log::indent() << "index: " << data.getRuleIndex() << ",\n"
	   << Log::indent() << "ptr: " << &data << ",\n"
	   << Log::indent() << "content: " << data.getPrintableValue() << "\n"
	   << Log::indent() << "ptr_next: " << data.getNextRule() << "\n";

	Log::decrementIndent();
	os << Log::indent() << "}";
	return os;
}