#include "ConfigManager.hpp"
#include <stdexcept>
#include "FileManager.hpp"
#include "Logger.hpp"
#include "EventLoop.hpp"

#define BNF_FILE_PATH "config/conf.bnf"
#define DEFAULT_CONFIG_PATH "config/default.conf"
#define DEFAULT_CONFIG_MSG "No configuration file specified. Using default: " DEFAULT_CONFIG_PATH

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		Logger::info(DEFAULT_CONFIG_MSG);
	}
	try
	{
		const std::string filepath = argc > 1 ? argv[1] : DEFAULT_CONFIG_PATH;

		const std::string bnf	 = FileManager::readFile(BNF_FILE_PATH);
		const std::string config = FileManager::readFile(filepath);

		ConfigManager configManager(bnf, config);
		configManager.lex();
		configManager.parse();
		return 0;
	}
	catch (const std::exception &e)
	{
		Logger::log("Fatal error: " + std::string(e.what()));
		return 1;
	}
	EventLoop::init();
	EventLoop::run();
	return 0;
}