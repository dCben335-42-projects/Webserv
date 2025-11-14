#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <string>
#include "../ARule.hpp"

using bnf::SYMBOL;
class Symbol : public ARule {
    public:
        Symbol();
        Symbol(const std::string& val);
        ~Symbol();
        void addItem(const std::string& val);
        std::vector<std::string>  getItems() const;
        const std::string& getValue() const;
        void print() const;
    private:
        std::string value;
};


#endif