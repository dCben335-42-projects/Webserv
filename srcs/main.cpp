#include "Lexer.hpp"
#include "EventLoop.hpp"
#include "Json.hpp"
#include "ConfigParser.hpp"

// clang-format on

int main()
{
	try
	{
		Lexer	   lexer("config/conf_test.txt");
		HttpConfig config = lexer.parseConfigFile("config/default.txt");

		// std::ifstream file("config/default.txt");

		// if (!file.is_open())
		// {
		// 	std::cerr << "Error: cannot open config file" << std::endl;
		// 	return 1;
		// }

		// ConfigParser parser;
		// HttpConfig	 config = parser.parse(file);

		// printHttpConfig(config);
		Logger::log(config);

		std::cout << "Parsed successfully!" << std::endl;
		std::cout << "Servers count: " << config.getServers().size() << std::endl;
		// Lexer lexer("config/config_nginx.txt");
		// lexer.parseConfigFile("config/test.txt");

		return 0;
	}
	catch (const std::exception &e)
	{
		Logger::log("Fatal error: " + std::string(e.what()));
	}
}