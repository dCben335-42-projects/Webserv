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



const std::set<ARule *> *ARule::getNextRules(Vec<ARule *> supRuleParent) const
{
	if (!m_nextRules.empty())
		return &m_nextRules;
	if (supRuleParent.size() == 1)
		return NULL;
	ARule *val = supRuleParent.back();
	supRuleParent.pop_back();
	return val->getNextRules(supRuleParent);
}

std::set<ARule *> *ARule::getNextRules(Vec<ARule *> supRuleParent)
{
	if (m_nextRules.empty())
	{
		if (supRuleParent.size() == 0)
		{
			return NULL; // Nous somme revenu sur <head> donc nous avons fini
		}
		else
		{
			ARule *val = supRuleParent.back();
			supRuleParent.pop_back();
			return val->getNextRules(supRuleParent);
		}
	}
	return &m_nextRules;
}

void ARule::printNext() const
{
	printSet(m_nextRules);
}

void ARule::setNextRules(ARule *nextRule)
{
	m_nextRules.insert(nextRule);
}

bool ARule::isTerminalCharAt(size_t index) const
{
	if (!m_terminalChars.has(index))
		return false;
	return m_terminalChars[index];
}

std::ostream &operator<<(std::ostream &os, const ARule &data)
{
	os << Log::indent() << "{\n";
	Log::incrementIndent();
	os << Log::indent() << "position: " << data.getConceptName() << "_" << data.getConceptDefIndex() << ",\n"
	   << Log::indent() << "type: " << data.getTypeName() << ",\n"
	   << Log::indent() << "index: " << data.getRuleIndex() << ",\n"
	   << Log::indent() << "ptr: " << &data << ",\n"
	   << Log::indent() << "content: " << data.getPrintableValue() << "\n";

	data.printNext();

	Log::decrementIndent();
	os << Log::indent() << "}";
	return os;
}