#ifndef OPTION_HPP
#define OPTION_HPP

#include <vector>
#include <string>
#include "bnf_defs.hpp"
#include "ARule.hpp"

using bnf::OPTION;

class Option : public ARule {
    public:
        Option();
        ~Option();
        void addItem(const std::string& val);
        void print() const;
    private:
        std::vector<std::string> items;
};
#endif