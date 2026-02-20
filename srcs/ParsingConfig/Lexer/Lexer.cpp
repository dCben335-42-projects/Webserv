#include "Lexer.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "Logger.hpp"

using namespace bnf;

void readFile(const std::string &filepath, std::string &content);


Lexer::Lexer(const std::string &content) : m_semanticDescriptor(content)
{
}

Lexer::~Lexer()
{
}



HttpConfig Lexer::parseConfigFile(std::string const &filepath)
{
	std::string content;
	readFile(filepath, content);

	Logger::log("Parsing config file: " + filepath);
	try
	{
		Parser sparser(m_semanticDescriptor);
		sparser.parse(content);
		Logger::log("Config file parsed successfully!");

		ConfigParser parser;
		return parser.parse(content);
	}
	catch (const SemanticException &e)
	{
		std::cout << e;
	}
	return HttpConfig();
}
