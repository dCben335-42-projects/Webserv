#include "./Symbol.hpp"
#include <iostream>

Symbol::Symbol(): ARule(SYMBOL) {}

Symbol::Symbol(const std::string& val): ARule(SYMBOL) {
    value = val;
}

Symbol::~Symbol() {}

void Symbol::print() const
{
    std::cout << "{ Symbol: " << value << "}"  << std::endl;
}

void Symbol::addItem(const std::string& val) {
    value = val;
}