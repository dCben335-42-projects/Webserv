#include "Lexer.hpp"
#include "EventLoop.hpp"

int main()
{
	try
	{
		Lexer lexer("config/config_nginx.txt");
		lexer.parseConfigFile("config/test.txt");
		return 0;
	}
	catch (const std::exception &e)
	{
		Logger::log("Fatal error: " + std::string(e.what()));
	}
}