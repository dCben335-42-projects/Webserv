#ifndef ARULE_HPP
#define ARULE_HPP

#include "bnf_defs.hpp"

using bnf::ERuleType;

class ARule {
    protected:
        const ERuleType m_type;
        ARule(ERuleType t);
    public:
        virtual ~ARule();

        ERuleType getType() const;
        virtual void print() const = 0;
};


#endif