#include "Lexer.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "bnf.hpp"

using bnf::SemanticDescriptor;

Lexer::Lexer(std::string const& filepath) {
    std::ifstream ifs(filepath.c_str(), std::ios::in);
    if (!ifs.is_open()) {
        throw std::runtime_error("Lexer: cannot open file: " + filepath);
    }
    std::ostringstream ss;
    ss << ifs.rdbuf();
    m_content = ss.str();

    SemanticDescriptor val = SemanticDescriptor();
    val.createBnf(m_content);

    val.printSemanticDescriptor();
    // val.checkValidity();





    ifs.close();
}

Lexer::~Lexer() {
    // No special cleanup required; m_content will be destroyed automatically
}

// std::string parseConceptName(const std::string& m_content) {
//     size_t pos = m_content.find("::=");
//     if (pos == std::string::npos)
//         throw std::runtime_error("Erreur: '::=' non trouvé");

//     std::string before = m_content.substr(0, pos);

//     before.erase(0, before.find_first_not_of(" \t\n\r"));
//     before.erase(before.find_last_not_of(" \t\n\r") + 1);

//     std::istringstream iss(before);
//     std::vector<std::string> parts;
//     std::string word;
//     while (iss >> word) {
//         parts.push_back(word);
//     }

//     if (parts.size() != 1) {
//         throw std::runtime_error("Erreur: clé invalide (espaces internes détectés)");
//     }

//     return before;
// }

// std::string parseRule(const std::string& m_content)
// {

// }




// void Lexer::createBnf()
// {

//     // std::cout << parseConceptName(m_content) << std::endl;


// }


const std::string& Lexer::getContent() const {
    return m_content;
}