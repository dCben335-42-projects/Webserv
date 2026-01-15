#include "Lexer.hpp"
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
		Logger::log("Fatal error: " + std::string(e.what()));
	}
}