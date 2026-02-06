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
	ifs.close();

	m_semanticDescriptor = new SemanticDescriptor(m_content);
	// Logger::log(m_semanticDescriptor);


	m_parser = new Parser(*m_semanticDescriptor);
}


void Lexer::parseConfigFile(std::string const &filepath)
{
	std::ifstream ifs(filepath.c_str(), std::ios::in);
	if (!ifs.is_open())
		throw std::runtime_error("Lexer: cannot open file: " + filepath);
	std::ostringstream ss;
	ss << ifs.rdbuf();
	m_content = ss.str();
	ifs.close();

	try
	{
		m_parser->parse(m_content);
	}
	catch (const SemanticException &e)
	{
		std::cout << e;
	}
}


Lexer::~Lexer()
{
}

const std::string &Lexer::getContent() const
{
	return m_content;
}
