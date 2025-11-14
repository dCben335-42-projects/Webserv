#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <string>
#include "ARule.hpp"

using bnf::SYMBOL;
class Symbol : public ARule {
    public:
        Symbol();
        Symbol(const std::string& val);
        ~Symbol();
        void addItem(const std::string& val);
        const std::string& getValue() const;
        void print() const;
    private:
        std::string value;
};


#endif