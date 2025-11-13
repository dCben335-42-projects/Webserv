#include "Concept.hpp"
#include <iostream>

Concept::Concept() {}

Concept::~Concept() {}

void Concept::setRule(ARule * rule) {
    values.push_back(rule);
}

void Concept::printRules() {
    for (std::vector<ARule *>::iterator it = values.begin(); it != values.end(); ++it) {
        ARule* rule = *it;
        if (rule)
            rule->print();
    }
}
