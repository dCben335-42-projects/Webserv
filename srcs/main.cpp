#include "ConfigManager.hpp"
#include <stdexcept>
#include "FileManager.hpp"
#include "Logger.hpp"
#include "EventLoop.hpp"
#include "webserv.hpp"


#include "SocketAddress.hpp"

/**
 *
 * @brief Reads the BNF and configuration files and stores their contents in the provided string references.
 *
 * @param bnf A reference to a string where the contents of the BNF file will be stored.
 * @param config A reference to a string where the contents of the configuration file will be stored.
 *
 * @return true if both files were read successfully, false if an error occurred while reading either file.
 *
 */
bool getFiles(std::string &bnf, std::string &config, const std::string &configPath)
{
	try
	{
		bnf	   = FileManager::readFile(BNF_FILE_PATH);
		config = FileManager::readFile(configPath);
	}
	catch (const FileManager::FileException &e)
	{
		Logger::error("Error reading file: " + std::string(e.what()));
		return false;
	}
	return true;
}


int main(int argc, char *argv[])
{
	if (argc < 2)
		Logger::info(DEFAULT_CONFIG_MSG);
	Logger::setCustomPrefix("[ " SERVER_FULL_NAME " ] ");
	const std::string &configPath = argc >= 2 ? argv[1] : DEFAULT_CONFIG_PATH;
	std::string		   bnf;
	std::string		   config;
	if (!getFiles(bnf, config, configPath))
		return 1;
	ConfigManager configManager(bnf, config);
	try
	{
		configManager.lex();
		configManager.parse();
	}
	catch (const std::exception &e)
	{
		Logger::log("Fatal error: " + std::string(e.what()));
		return 1;
	}
	const VServersConfigs &serversConfigs = configManager.getServersConfigs();
	const Config		  &httpConfig	  = configManager.getHttpConfig();
	EventLoop::init(httpConfig, serversConfigs);
	EventLoop::run();
	EventLoop::destroy();
	return 0;
}