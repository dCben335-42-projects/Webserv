#include "Option.hpp"
#include <iostream>

Option::Option() : ARule(OPTION) {}
Option::~Option() {}

void Option::addItem(const std::string& val) {
    items.push_back(val);
}

void Option::print() const {
    std::cout << "{ Option: [";
    for (size_t i = 0; i < items.size(); ++i) {
        if (i != 0)
            std::cout << ", ";
        std::cout << items[i];
    }
    std::cout << "]}" << std::endl;
}