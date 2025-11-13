#include "ARule.hpp"
#include "Group.hpp"
#include "Sequence.hpp"
#include "Option.hpp"
#include "Symbol.hpp"

using namespace bnf;

ARule::ARule(ERuleType t) : m_type(t) {}

ARule::~ARule() {}

ARule& ARule::operator=(const ARule &other) {
    if (this == &other)
        return *this;

    return *this;
}

ERuleType ARule::getType() const {
    return m_type;
}

ARule* createGroup() { return new Group(); }

ARule* createSequence() { return new Sequence(); }

ARule* createOption() { return new Option(); }

ARule* createSymbol() { return new Symbol(); }




static const BnfMap rules[] = {
    {
        .opener = LBRACE,
        .closer = RBRACE,
        .constructor = &createGroup
    },
    {
        .opener = LBRACKET,
        .closer = RBRACKET,
        .constructor = &createSequence
    },
     {
        .opener = LHOOK,
        .closer = RHOOK,
        .constructor = &createOption
    },
    {
        .constructor = NULL
    },
};

BnfMap ARule::getMap(const std::string &opener)
{
    for (size_t i = 0; rules[i].constructor; i++)
        if (opener == rules[i].opener)
            return rules[i];
    return (BnfMap){
        .opener = "",
        .closer = "",
        .constructor = &createSymbol
    };
}