#include "ARule.hpp"

ARule::ARule(ERuleType t) : m_type(t) {}

ARule::~ARule() {}

ERuleType ARule::getType() const {
    return m_type;
}