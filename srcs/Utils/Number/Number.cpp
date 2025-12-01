#include "Number.hpp"
#include <sstream>
#include <cstdlib>

Number::Number() {}
Number::~Number() {}

std::string Number::toString(const int number)
{
    std::ostringstream  oss;
    oss << number;
    return oss.str();
}

int Number::fromString(const std::string &str) {
    return std::atoi(str.c_str());
}