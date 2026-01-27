#include <string>
#include <memory>
#include <iostream>
#include <map>
#include <vector>
#include "bnf.hpp"

class Lexer
{
	private:
		std::string			m_content;
		SemanticDescriptor *m_semanticDescriptor;
		Parser			   *m_parser;

	public:
		Lexer(std::string const &filepath);
		~Lexer();
		void parseConfigFile(std::string const &filepath);

		const std::string &getContent() const;
};