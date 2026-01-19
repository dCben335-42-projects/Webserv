#include "ARule.hpp"

#include "./Group/Group.hpp"
#include "./Sequence/Sequence.hpp"
#include "./Option/Option.hpp"
#include "./Symbol/Symbol.hpp"
#include "../Concept/Concept.hpp"

#include "Logger.hpp"

using namespace bnf;

ARule::ARule(ERuleType t, const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr) : m_type(t), m_conceptName(conceptName), m_conceptDefIndex(conceptDefIndex), m_ruleIndex(ruleIndex), m_conceptPtr(conceptPtr)
{
}

ARule::~ARule()
{
}

ERuleType ARule::getType() const
{
	return m_type;
}

ARule *createGroup(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr)
{
	return new Group(conceptName, conceptDefIndex, ruleIndex, conceptPtr);
}

ARule *createSequence(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr)
{
	return new Sequence(conceptName, conceptDefIndex, ruleIndex, conceptPtr);
}

ARule *createOption(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr)
{
	return new Option(conceptName, conceptDefIndex, ruleIndex, conceptPtr);
}

ARule *createSymbol(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr)
{
	return new Symbol(conceptName, conceptDefIndex, ruleIndex, conceptPtr);
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

ARulePath ARule::getNextRules(ARulePath supRuleParent)
{
	if (m_nextRules.empty())
	{
		if (supRuleParent.size() == 1)
		{
			return ARulePath();
		}
		else
		{
			supRuleParent.pop_back();
			ARule *val = supRuleParent.back();
			return val->getNextRules(supRuleParent);
		}
	}
	supRuleParent.back() = *m_nextRules.begin();
	return supRuleParent;
}

ARule *ARule::tryGetAdjacentRules()
{
	Set<ARule *>::iterator it = m_nextRules.begin();
	return *it;
}

std::string ARule::printNext() const
{
	return LoggerJson::get(m_nextRules, true);
}

void ARule::setNextRules(ARule *nextRule)
{
	m_nextRules.insert(nextRule);
}

Concept *ARule::getConceptPtr() const
{
	return m_conceptPtr;
}

std::ostream &operator<<(std::ostream &os, const ARule &data)
{
	os << "{\n";
	Logger::incrementIndent();
	os << Logger::indent() << "position: " << data.getConceptName() << "_" << data.getConceptDefIndex() << ",\n"
	   << Logger::indent() << "type: " << data.getTypeName() << ",\n"
	   << Logger::indent() << "index: " << data.getRuleIndex() << ",\n"
	   << Logger::indent() << "ptr: " << &data << ",\n"
	   << Logger::indent() << "content: " << data.getPrintableValue() << "\n"
	   << Logger::indent() << "next: " << data.printNext() << "\n";
	Logger::decrementIndent();
	os << Logger::indent() << "}";
	return os;
}