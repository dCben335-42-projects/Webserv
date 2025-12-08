#ifndef GROUP_HPP
#define GROUP_HPP

#include <vector>
#include <string>
#include "bnf_defs.hpp"
#include "../ARule.hpp"

using bnf::GROUP;

class Group : public ARule {
    public:
        Group();
        ~Group();
        void addItem(const std::string& val);
        void print() const;
    private:
        std::vector<std::string> items;
};
#endif