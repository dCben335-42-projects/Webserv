#include "Lexer.hpp"
#include "EventLoop.hpp"
#include "String.hpp"

int main()
{
	try
	{
		Logger::setFormat(Logger::Json);
		EventLoop::init();
		EventLoop::run();
		return 0;
	}
	catch (const std::exception &e)
	{
		Logger::log("Fatal error: " + std::string(e.what()));
	}
}