#include "Lexer.hpp"
#include "Logger.hpp"
#include "config.hpp"
#include "EventLoop.hpp"

int main()
{
	try
	{
		EventLoop::init();
		EventLoop::run();
	}
	catch (const std::exception &e)
	{
		log("Fatal error: " + std::string(e.what()));
	}
}