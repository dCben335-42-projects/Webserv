#include "Lexer.hpp"
#include "EventLoop.hpp"
#include "Json.hpp"
#include "ConfigParser.hpp"
#include <sstream>
#include <fstream>
#include <stdexcept>

// clang-format on

void readFile(const std::string &filepath, std::string &content)
{
	std::ifstream ifs(filepath.c_str(), std::ios::in);
	if (!ifs.is_open())
		throw std::runtime_error("Cannot open file: " + filepath);
	std::ostringstream ss;
	ss << ifs.rdbuf();
	content = ss.str();
	ifs.close();
}


int main()
{
	try
	{
		std::string content;
		readFile("config/conf_test.txt", content);
		Lexer lexer(content);

		Logger::log("Starting config parsing...");

		HttpConfig config = lexer.parseConfigFile("config/default.txt");


		Logger::log(config);

		// std::cout << "Parsed successfully!" << std::endl;
		// std::cout << "Servers count: " << config.getServers().size() << std::endl;
		// Lexer lexer("config/config_nginx.txt");
		// lexer.parseConfigFile("config/test.txt");

		return 0;
	}
	catch (const std::exception &e)
	{
		Logger::log("Fatal error: " + std::string(e.what()));
	}
}