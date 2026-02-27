#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

#include "Parser.hpp"
#include "Config.hpp"
#include "bnf.hpp"

class ConfigManager
{
	private:
		::Parser		   m_parser;
		const std::string &m_fileContent;
		const std::string &m_bnfContent;

	public:
		ConfigManager(const std::string &bnfFileContent, const std::string &configFileContent);
		~ConfigManager();

		const Config		  &getHttpConfig() const;
		const VServersConfigs &getServersConfigs() const;


		void lex();
		void parse();

		HttpNode parseConfigFile(std::string const &filepath);
};

#endif
