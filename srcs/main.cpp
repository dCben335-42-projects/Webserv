#include "Lexer.hpp"
#include "config.hpp"
#include "EventLoop.hpp"


int main()
{
	try
	{
		Lexer lex("config/def_language.txt");
		Logger::FileMode mode = Logger::Append;
	}
	catch (const std::exception &e)
	{
		Logger::log("Fatal error: " + std::string(e.what()));
	}
}