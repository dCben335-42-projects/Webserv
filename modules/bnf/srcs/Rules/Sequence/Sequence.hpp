#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

#include <vector>
#include <string>
#include "bnf_defs.hpp"
#include "../ARule.hpp"

using bnf::SEQUENCE;

class Sequence : public ARule {
    public:
        Sequence();
        ~Sequence();
        void addItem(const std::string& val);
        std::vector<std::string>  getItems() const;
        void print() const;
    private:
        std::vector<std::string> items;
};

#endif