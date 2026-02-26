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
	m_parser.resolve();
	m_parser.removeDuplicate();
	m_parser.createConfigs();
	const ::Parser::VServerConfigs &serversConfigs = m_parser.getServersConfigs();
	Logger::debug("Parsed " + Logger::get(serversConfigs.size()) + " server configurations:");
	Logger::log(serversConfigs);
	const Config &httpConfig = m_parser.getHttpConfig();
	Logger::debug("Parsed HTTP configuration:");
	Logger::log(httpConfig);
}
