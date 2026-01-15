#include "ARule.hpp"

#include "./Group/Group.hpp"
#include "./Sequence/Sequence.hpp"
#include "./Option/Option.hpp"
#include "./Symbol/Symbol.hpp"

#include "Logger.hpp"

using namespace bnf;

ARule::ARule(ERuleType t) : m_type(t)
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

ARule *createGroup()
{
	return new Group();
}

ARule *createSequence()
{
	return new Sequence();
}

ARule *createOption()
{
	return new Option();
}

ARule *createSymbol()
{
	return new Symbol();
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

std::ostream &operator<<(std::ostream &os, const ARule &data)
{
	os << Logger::indent() << data.getPrintableValue();
	return os;
}