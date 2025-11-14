#include "./Symbol.hpp"
#include <iostream>

Symbol::Symbol(): ARule(SYMBOL) {}

Symbol::Symbol(const std::string& val): ARule(SYMBOL) {
    value = val;
}

Symbol::~Symbol() {}

std::vector<std::string> Symbol::getItems() const {
    std::vector<std::string> items;
    items.push_back(value);
    return items;
}

void Symbol::print() const
{
    std::cout << "{ Symbol: " << value << "}"  << std::endl;
}

void Symbol::addItem(const std::string& val) {
    value = val;
}