#include "ARule.hpp"

#include "./Group/Group.hpp"
#include "./Sequence/Sequence.hpp"
#include "./Option/Option.hpp"
#include "./Symbol/Symbol.hpp"
#include "../Concept/Concept.hpp"

#include "Logger.hpp"

using namespace bnf;

ARule::ARule(
	ERuleType		   t,
	const std::string &conceptName,
	size_t			   conceptDefIndex,
	size_t			   ruleIndex,
	Concept			  *conceptPtr,
	ConceptDef		  *conceptDefPtr) :
	m_type(t),
	m_conceptName(conceptName),
	m_conceptDefIndex(conceptDefIndex),
	m_ruleIndex(ruleIndex),
	m_conceptPtr(conceptPtr),
	m_conceptDefPtr(conceptDefPtr)
{
}

ARule::~ARule()
{
}

ERuleType ARule::getType() const
{
	return m_type;
}

ARule *createGroup(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr, ConceptDef *conceptDefPtr)
{
	return new Group(conceptName, conceptDefIndex, ruleIndex, conceptPtr, conceptDefPtr);
}

ARule *createSequence(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr, ConceptDef *conceptDefPtr)
{
	return new Sequence(conceptName, conceptDefIndex, ruleIndex, conceptPtr, conceptDefPtr);
}

ARule *createOption(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr, ConceptDef *conceptDefPtr)
{
	return new Option(conceptName, conceptDefIndex, ruleIndex, conceptPtr, conceptDefPtr);
}

ARule *createSymbol(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr, ConceptDef *conceptDefPtr)
{
	return new Symbol(conceptName, conceptDefIndex, ruleIndex, conceptPtr, conceptDefPtr);
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

/**
 * @brief Check if a word is a keyword (opener or closer)
 *
 * @param word The word to check
 * @param onlyClose If true, only check for closers
 * @return true if the word is a keyword, false otherwise
 */
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

/**
 * @brief Get the BnfMap for a given opener
 *
 * @param opener The opener to get the BnfMap for
 * @return BnfMap The BnfMap for the given opener
 */
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

ConceptDef *ARule::getConceptDefPtr()
{
	return m_conceptDefPtr;
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

ARulePath ARule::getNextRules(ARulePath supRuleParent) const
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
			const ARule *val = supRuleParent.back();
			return val->getNextRules(supRuleParent);
		}
	}
	supRuleParent.back() = *m_nextRules.begin();
	return supRuleParent;
}

const ARule *ARule::tryGetAdjacentRules() const
{
	Set<const ARule *>::iterator it = m_nextRules.begin();
	return *it;
}

std::string ARule::printNext() const
{
	return LoggerJson::get(m_nextRules, true);
}

void ARule::setNextRules(const ARule *nextRule)
{
	m_nextRules.insert(nextRule);
}

Concept *ARule::getConceptPtr() const
{
	return m_conceptPtr;
}

bool ARule::isOption() const
{
	return m_type == OPTION;
}

bool ARule::isSequence() const
{
	return m_type == SEQUENCE;
}

const Map<size_t, Vec<std::string>> &ARule::getRequiredInJsonMap() const
{
	return m_requiredInJson;
}

void ARule::addRequiredInJson(size_t index, const std::string &val)
{
	if (!m_requiredInJson.has(index))
		m_requiredInJson.set(index, Vec<std::string>());
	m_requiredInJson.get(index).push(val);
}

std::ostream &operator<<(std::ostream &os, const ARule &data)
{
	os << "{\n";
	Logger::incrementIndent();
	os << Logger::indent() << "\"position\": " << "\"" << data.getConceptName() << "_" << data.getConceptDefIndex() << "\",\n";
	os << Logger::indent() << "\"type\": " << "\"" << data.getTypeName() << "\",\n";
	os << Logger::indent() << "\"index\": " << data.getRuleIndex() << ",\n";
	os << Logger::indent() << "\"is_required_in_json\": " << LoggerJson::get(data.getRequiredInJsonMap()) << ",\n";
	os << Logger::indent() << "\"ptr\": " << "\"" << &data << "\",\n";
	os << Logger::indent() << "\"content\": " << data.getPrintableValue() << "\n";
	os << Logger::indent() << "\"next\": " << data.printNext() << "\n";
	os << Logger::indent() << "items\": " << LoggerJson::get(data.getItems()) << "\n";
	Logger::decrementIndent();
	os << Logger::indent() << "}";
	return os;
}