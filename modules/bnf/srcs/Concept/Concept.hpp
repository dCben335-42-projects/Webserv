#ifndef CONCEPT_HPP
#define CONCEPT_HPP

#include <vector>
#include <string>
#include "bnf_defs.hpp"
#include "../Rules/ARule.hpp"


class Concept {
    public:
    Concept();
    ~Concept();
    
    void setRule(ARule * rule);
    void printRules() const;
    std::vector<ARule *> getRules();
    bool calledInConcept(std::string conceptName);
    std::vector<std::string> getItems();
    std::vector<ARule *> getValues() const;
    private:
    std::vector<ARule *> values;
};

std::ostream& operator<<(std::ostream& os, const Concept& data);

typedef std::vector<Concept *> ConceptList;

#endif
