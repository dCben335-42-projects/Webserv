#ifndef CONCEPT_HPP
#define CONCEPT_HPP

#include <vector>
#include <string>
#include "bnf_defs.hpp"
#include "ARule.hpp"

class Concept {
    public:
        Concept();
        ~Concept();

        void setRule(ARule * rule);
        void printRules();
    private:
        std::vector<ARule *> values;
};

#endif
