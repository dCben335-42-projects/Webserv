#ifndef ARULE_HPP
#define ARULE_HPP

#include "bnf_defs.hpp"
#include <vector>

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
        ERuleType getType() const;
        
        virtual void print() const = 0;
        virtual std::vector<std::string>  getItems() const = 0;
        virtual std::string getPrintableValue() const = 0;
        
        static BnfMap   getMap(const std::string &opener);
        static bool     isKeyWord(const std::string &word, bool onlyClose);
};

std::ostream& operator<<(std::ostream& os, const ARule& data);

struct BnfMap 
{
    std::string opener;
    std::string closer;
    ARule* (*constructor)();
    bool       hasClosure;
};

#endif