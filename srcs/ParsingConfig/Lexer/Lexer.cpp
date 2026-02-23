#include "Lexer.hpp"
#include <stdexcept>
#include <iostream>
#include "Logger.hpp"
#include "FileManager.hpp"

Lexer::Lexer(const std::string &content) : m_semanticDescriptor(content)
{
}

Lexer::~Lexer()
{
}


HttpConfig Lexer::parseConfigFile(std::string const &filepath)
{
	std::string content = FileManager::readFile(filepath);

	Parser sparser(m_semanticDescriptor);
	sparser.parse(content);
	Logger::log("Config file parsed successfully!");


	ConfigParser parser;
	return parser.parse(content);
}
