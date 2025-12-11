#include <sstream>
#include <iostream>
#include "./Parser.hpp"
#include "Vector.hpp"
#include "Logger.hpp"

Parser::Parser(const SemanticDescriptor& semantic) : m_semantic(semantic) {}

Parser::~Parser() {}

void Parser::parse(const std::string& stringToParsed) {
    for (std::string::const_iterator it = stringToParsed.begin(); it != stringToParsed.end(); ++it) {
        char c = *it;

        std::cout << c;
    }
}