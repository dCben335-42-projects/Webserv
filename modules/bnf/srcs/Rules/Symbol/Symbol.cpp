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

const std::string& Symbol::getValue() const {
    return value;
}

std::string Symbol::getPrintableValue() const {
    std::string s = std::string("{Symbol: ") + value + "}";
    return s;
}

std::ostream& operator<<(std::ostream& os, const Symbol& data)
{
    os << data.getPrintableValue();
    return os;
}