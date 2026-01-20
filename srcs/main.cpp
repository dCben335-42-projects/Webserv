#include "Lexer.hpp"
#include "config.hpp"
#include "EventLoop.hpp"


int main()
{
	try
	{
		// Lexer lexer("config/def_language.txt");
		EventLoop::init();
		EventLoop::run();
		EventLoop::destroy();
		return 0;
	}
	catch (const std::exception &e)
	{
		Logger::log("Fatal error: " + std::string(e.what()));
	}
}