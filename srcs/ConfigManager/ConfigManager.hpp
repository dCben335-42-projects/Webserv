#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

#include "Parser.hpp"
#include "bnf.hpp"

class ConfigManager
{
	private:
		bnf::SemanticDescriptor m_semanticDescriptor;
		::Parser				m_parser;

		const std::string &m_content;

	public:
		ConfigManager(const std::string &bfnFileContent, const std::string &configFileContent);
		~ConfigManager();

		void lex();
		void parse();

		HttpConfig parseConfigFile(std::string const &filepath);
};

#endif
