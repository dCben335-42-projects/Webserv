#include "./Sequence.hpp"
#include <iostream>

Sequence::Sequence() : ARule(SEQUENCE) {}

Sequence::~Sequence() {}

void Sequence::addItem(const std::string& val) {
    items.push_back(val);
}

void Sequence::print() const {
    std::cout << "{ Sequence: [";
    for (size_t i = 0; i < items.size(); ++i) {
        if (i != 0)
            std::cout << ", ";
        std::cout << items[i];
    }
    std::cout << "]}" << std::endl;
    // for (size_t i = 0; i < items.size(); ++i) {
    //     std::cout << items[i] << std::endl;
    // }
}