#include "Group.hpp"
#include <iostream>

Group::Group() : ARule(GROUP) {}
Group::~Group() {}

void Group::addItem(const std::string& val) {
    items.push_back(val);
}

void Group::print() const {
    for (size_t i = 0; i < items.size(); ++i) {
        std::cout << items[i] << std::endl;
    }
}