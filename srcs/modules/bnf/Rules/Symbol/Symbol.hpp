#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <string>
#include "bnf_defs.hpp"
#include "ARule.hpp"

using bnf::SYMBOL;
class Symbol : public ARule {
    public:
        Symbol();
        ~Symbol();
        void setValue(const std::string& val);
        const std::string& getValue() const;
        void print() const;
    private:
        std::string value;
};


#endif