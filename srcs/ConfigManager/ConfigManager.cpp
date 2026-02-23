#include "ConfigManager.hpp"
#include "Logger.hpp"
#include "FileManager.hpp"

ConfigManager::ConfigManager(const std::string &bfnFileContent, const std::string &configFileContent) :
	m_semanticDescriptor(bfnFileContent),
	m_parser(configFileContent),
	m_content(configFileContent)
{
}

ConfigManager::~ConfigManager()
{
}



void ConfigManager::lex()
{
	m_semanticDescriptor.parse();

	bnf::Parser bnfParser(m_semanticDescriptor);
	bnfParser.parse(m_content);
}


void ConfigManager::parse()
{
	m_parser.retreive();
	m_parser.parse();
	Logger::debug("Parsed HTTP config: \n" + Logger::get(m_parser.getHttpConfig()));
}
