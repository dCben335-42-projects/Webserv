#include "Lexer.hpp"
#include <stdexcept>
#include "FileManager.hpp"
#include "Logger.hpp"
#include "EventLoop.hpp"

int main()
{
	try
	{
		const std::string content = FileManager::readFile("config/conf_test.txt");
		Lexer			  lexer(content);

		const HttpConfig config = lexer.parseConfigFile("config/default.txt");
		Logger::log(config);
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