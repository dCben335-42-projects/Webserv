#ifndef ARULE_HPP
#define ARULE_HPP

#include "bnf_defs.hpp"

using bnf::ERuleType;

struct BnfMap;

class ARule {
    protected:
    const ERuleType m_type;
    ARule(ERuleType t);
    public:
    virtual ~ARule();
    
    ARule& operator=(const ARule &other);
    
    virtual void addItem(const std::string& val) = 0;
    
    static BnfMap getMap(const std::string &opener);
    
    ERuleType getType() const;
    virtual void print() const = 0;
};

struct BnfMap 
{
    std::string opener;
    std::string closer;
    ARule* (*constructor)();
};

#endif