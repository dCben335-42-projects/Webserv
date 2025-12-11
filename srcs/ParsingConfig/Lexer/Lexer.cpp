#include "Lexer.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "Logger.hpp"

using namespace bnf;

Lexer::Lexer(std::string const &filepath)
{
	std::ifstream ifs(filepath.c_str(), std::ios::in);
	if (!ifs.is_open())
		throw std::runtime_error("Lexer: cannot open file: " + filepath);
	std::ostringstream ss;
	ss << ifs.rdbuf();
	m_content = ss.str();

	SemanticDescriptor sdawd = SemanticDescriptor(m_content);
	log(sdawd);
	std::cout << std::endl;
	ifs.close();
}

Lexer::~Lexer()
{
}

const std::string &Lexer::getContent() const
{
	return m_content;
}
