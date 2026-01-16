#include "Lexer.hpp"
#include "config.hpp"
#include "EventLoop.hpp"


int main()
{
	try
	{
		// Lexer lex("config/def_language.txt");
		EventLoop::init();
		EventLoop::run();
		return 0;
	}
	catch (const std::exception &e)
	{
		Logger::log("Fatal error: " + std::string(e.what()));
	}
}