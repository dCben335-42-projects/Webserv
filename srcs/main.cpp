#include "Lexer.hpp"
#include "EventLoop.hpp"

int main()
{
	try
	{
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