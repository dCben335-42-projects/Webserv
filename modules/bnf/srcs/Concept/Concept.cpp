#include "Concept.hpp"
#include <iostream>

using namespace bnf;

Concept::Concept() {}

Concept::~Concept() {}

void Concept::setRule(ARule * rule) {
    values.push_back(rule);
}

std::vector<ARule *> Concept::getRules() {
    return values;
}

bool Concept::calledInConcept(std::string conceptName) {
    for (std::vector<ARule *>::iterator it = values.begin(); it != values.end(); ++it) {
        ARule* rule = *it;
        std::vector<std::string> items = rule->getItems();
        if (rule->getType() != OPTION)
            continue;
        for (std::vector<std::string>::iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt) {
            if (*itemIt == conceptName) {
                return true;
            }
        }
    }
    return false;
}

std::vector<std::string> Concept::getItems() {
    std::vector<std::string> allItems;
    for (std::vector<ARule *>::iterator it = values.begin(); it != values.end(); ++it) {
        ARule* rule = *it;
        if (rule->getType() != OPTION)
            continue;
        std::vector<std::string> items = rule->getItems();
        allItems.insert(allItems.end(), items.begin(), items.end());
    }
    return allItems;
}

void Concept::printRules() {
    for (std::vector<ARule *>::iterator it = values.begin(); it != values.end(); ++it) {
        ARule* rule = *it;
        if (rule)
            rule->print();
    }
}