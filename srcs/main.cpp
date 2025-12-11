#include "HttpServer.hpp"
#include "Lexer.hpp"
#include "Logger.hpp"
#include "config.hpp"

int main()
{
	try
	{
		Lexer	   lex("config/def_language.txt");
		HttpServer server(DEFAULT_PORT);
		server.run();
	}
	catch (const std::exception &e)
	{
		log("Fatal error: " + std::string(e.what()));
	}
}