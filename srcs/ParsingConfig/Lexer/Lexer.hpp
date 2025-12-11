#include <string>
#include <memory>
#include <iostream>
#include <map>
#include <vector>
#include "bnf.hpp"

class Lexer {
    private:
        std::string m_content;
        SemanticDescriptor m_semanticDescriptor;
    public:
        Lexer(std::string const& filepath);
        ~Lexer();

        const std::string& getContent() const;
};