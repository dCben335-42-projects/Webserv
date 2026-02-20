#include <string>
#include <memory>
#include <iostream>
#include <map>
#include <vector>
#include "bnf.hpp"
#include "ConfigParser.hpp"

class Lexer
{
	private:
		const SemanticDescriptor m_semanticDescriptor;

	public:
		Lexer(const std::string &content);
		~Lexer();
		HttpConfig parseConfigFile(std::string const &filepath);
};