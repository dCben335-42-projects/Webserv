#include "ConfigManager.hpp"
#include "Logger.hpp"
#include "FileManager.hpp"

ConfigManager::ConfigManager(const std::string &bnfFileContent, const std::string &configFileContent) :
	m_parser(configFileContent),
	m_fileContent(configFileContent),
	m_bnfContent(bnfFileContent)
{
}

ConfigManager::~ConfigManager()
{
}


/**
 *
 * @brief Gets the HTTP configuration.
 *
 * @return A constant reference to the HTTP configuration.
 *
 */
const Config &ConfigManager::getHttpConfig() const
{
	return m_parser.getHttpConfig();
}

/**
 *
 * @brief Gets the server configurations.
 *
 * @return A constant reference to the vector of server configurations.
 *
 */
const VServersConfigs &ConfigManager::getServersConfigs() const
{
	return m_parser.getServersConfigs();
}


/**
 *
 * @brief Lexical analysis of the configuration file using the BNF descriptor.
 *
 * This function performs lexical analysis on the configuration file content using the BNF descriptor.
 * It first parses the BNF content to create a semantic descriptor,
 * and then uses that descriptor to parse the configuration file content.
 *
 * @throws std::runtime_error if there is an error during parsing.
 *
 */
void ConfigManager::lex()
{
	bnf::SemanticDescriptor descriptorParser;
	descriptorParser.parse(m_bnfContent);

	bnf::Parser bnfParser(descriptorParser);
	bnfParser.parse(m_fileContent);
}


/**
 *
 * @brief Parses the configuration file and populates the server and HTTP configurations.
 *
 * This function performs the parsing of the configuration file content using the Parser class.
 * It retrieves the configuration data, resolves any references, removes duplicate server configurations,
 * and creates the final server and HTTP configurations.
 *
 * @throws std::runtime_error if there is an error during parsing or configuration creation.
 *
 */
void ConfigManager::parse()
{
	m_parser.retreive();
	m_parser.resolve();
	m_parser.removeDuplicate();
	m_parser.createConfigs();
}
