#include "Lexer.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

Lexer::Lexer(std::string const& filepath) {
    std::ifstream ifs(filepath.c_str(), std::ios::in);
    if (!ifs.is_open()) {
        throw std::runtime_error("Lexer: cannot open file: " + filepath);
    }
    std::ostringstream ss;
    ss << ifs.rdbuf();
    m_content = ss.str();
    ifs.close();
}

Lexer::~Lexer() {
    // No special cleanup required; m_content will be destroyed automatically
}

const std::string& Lexer::getContent() const {
    return m_content;
}