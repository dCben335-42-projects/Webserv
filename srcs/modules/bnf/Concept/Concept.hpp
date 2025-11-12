#ifndef CONCEPT_HPP
#define CONCEPT_HPP

#include <vector>
#include <string>
#include "bnf_defs.hpp"
#include "ARule.hpp"

class Concept {
    public:
        Concept(const std::string& k, const std::vector<ARule *>& vals);
        ~Concept();
    private:
        std::string key;
        std::vector<ARule *> values;
};

#endif
