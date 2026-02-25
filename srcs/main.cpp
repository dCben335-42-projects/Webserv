#include "ConfigManager.hpp"
#include <stdexcept>
#include "FileManager.hpp"
#include "Logger.hpp"
#include "EventLoop.hpp"

#define BNF_FILE_PATH "config/conf.bnf"
#define DEFAULT_CONFIG_PATH "config/default.conf"
#define DEFAULT_CONFIG_MSG "No configuration file specified. Using default: " DEFAULT_CONFIG_PATH


#include "SocketAddress.hpp"


int main(int argc, char *argv[])
{
	(void) argc;
	(void) argv;
	if (argc < 2)
	{
		Logger::info(DEFAULT_CONFIG_MSG);
	}
	try
	{
		const std::string filepath = argc > 1 ? argv[1] : DEFAULT_CONFIG_PATH;

		const std::string bnf	 = FileManager::readFile(BNF_FILE_PATH);
		const std::string config = FileManager::readFile(filepath);


		std::string ip1 = "4::16";
		std::string ip2 = "40::16";

		SocketAddress a("[4::16]", 8080);
		SocketAddress b("[4:0:0:0:0:0:0:16]", 8080);

		Logger::error("Comparing SocketAddress a and b: " + Logger::get(a) + " == " + Logger::get(b) + " ? " + (a == b ? "true" : "false"));
		ConfigManager configManager(bnf, config);
		configManager.lex();
		configManager.parse();
	}
	catch (const std::exception &e)
	{
		Logger::log("Fatal error: " + std::string(e.what()));
		return 1;
	}
	// EventLoop::init();
	// EventLoop::run();
	return 0;
}