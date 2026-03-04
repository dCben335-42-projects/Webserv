#include "ConfigManager.hpp"
#include <stdexcept>
#include "FileManager.hpp"
#include "Logger.hpp"
#include "EventLoop.hpp"
#include "webserv.hpp"
#include "Signals.hpp"
#include "EarlySignal.hpp"
#include "log.hpp"

static EarlySignal g_earlySignal;

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
		Logger::custom(LOG_MSG_ERROR_READING_FILE + std::string(e.what()));
		return false;
	}
	return true;
}


/**
 *
 * @brief Parses the configuration using the provided ConfigManager instance.
 *
 * @param configManager A reference to a ConfigManager instance that
 * has been initialized with the BNF and configuration file contents.
 *
 * @return true if the configuration was parsed successfully,
 * false if an error occurred during parsing.
 *
 */
bool parseConfig(ConfigManager &configManager)
{
	try
	{
		configManager.lex();
		configManager.parse();
	}
	catch (const std::exception &e)
	{
		Logger::custom(ERROR_MSG_CONFIG_PREFIX + std::string(e.what()));
		return false;
	}
	return true;
}


int main(int argc, char *argv[])
{
	Logger::setCustomPrefix("[ " SERVER_FULL_NAME " ] ").setFormat(Logger::Json);
	if (g_earlySignal.getCaught())
	{
		Logger::custom(LOG_MSG_SIGNAL_CAUGHT_BEFORE_INIT);
		return 1;
	}
	if (argc < 2)
		Logger::custom(DEFAULT_CONFIG_MSG DEFAULT_CONFIG_PATH);
	{
		const std::string &configPath = argc >= 2 ? argv[1] : DEFAULT_CONFIG_PATH;
		std::string		   bnf;
		std::string		   config;
		if (!getFiles(bnf, config, configPath))
			return 1;

		if (g_earlySignal.getCaught())
		{
			Logger::custom(LOG_MSG_SIGNAL_CAUGHT_BEFORE_INIT);
			return 1;
		}

		ConfigManager configManager(bnf, config);
		if (!parseConfig(configManager))
			return 1;

		const VServersConfigs &serversConfigs = configManager.getServersConfigs();
		const Config		  &httpConfig	  = configManager.getHttpConfig();
		if (FLAG_LOG_CONF)
		{
			Logger::custom("serversConfigs: " + Logger::get(serversConfigs));
			Logger::custom("httpConfig: " + Logger::get(httpConfig));
		}
		EventLoop::init(httpConfig, serversConfigs);
	}
	EventLoop::run();
	EventLoop::destroy();
	return 0;
}