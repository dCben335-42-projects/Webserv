#include "Concept.hpp"
#include <iostream>
#include <algorithm>
#include "../utils/utils.hpp"
#include "Logger.hpp"

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
        if (rule->getType() == OPTION)
            continue;
        std::vector<std::string> items = rule->getItems();

        for (size_t i = 0; i < items.size(); ++i) {
            if (std::find(allItems.begin(), allItems.end(), items[i]) == allItems.end())
                allItems.push_back(items[i]);
        }
    }

    // std::cout << allItems.size() << " items found in concept." << std::endl;
    return allItems;
}

void Concept::printRules() const {
    for (std::vector<ARule *>::const_iterator it = values.begin(); it != values.end(); ++it) {
        ARule* rule = *it;
        if (rule)
            rule->print();
    }
}

std::vector<ARule *> Concept::getValues() const {
    return values;
}

std::ostream& operator<<(std::ostream& os, const Concept& data)
{
    (void)os;
    // data.printRules();
    // (void)data;

    std::vector<ARule *> values = data.getValues();

    log(values);

    // for (std::vector<ARule *>::const_iterator it = values.begin(); it != values.end(); ++it) {
        // ARule* rule = *it;
        // os << *rule;
    // }

    return os;


    // os << "TreeNodeData(name: " << data.name << ", concepts count: " 
    //    << (data.concepts ? data.concepts->size() : 0) << ")";
    // return os;
}