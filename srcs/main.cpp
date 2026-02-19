#include "Lexer.hpp"
#include "EventLoop.hpp"
#include "Json.hpp"
#include "ConfigParser.hpp"
// void testJson()
// {
// 	JsonObject *obj = new JsonObject();

// 	obj->create("name", "John");
// 	obj->create("age", 30.0);
// 	obj->create("isStudent", false);

// 	Vec<AJson *> hobbies;
// 	hobbies.push(AJson::createString("reading"));
// 	hobbies.push(AJson::createString("coding"));
// 	obj->create("hobbies", hobbies);

// 	Vec<AJson *> address;
// 	address.push(AJson::createString("street"));
// 	address.push(AJson::createString("city"));
// 	address.push(AJson::createNumber(12345));
// 	address.push(AJson::createObject());
// 	JsonObject *nestedObj = address.get(3)->asObject();
// 	nestedObj->create("nestedKey", "nestedValue");
// 	nestedObj->create("nestedNumber", 42.0);

// 	obj->create("address", address);

// 	Logger::log(Logger::get(obj));
// 	delete obj;
// }

// clang-format off
void printDirectives(const std::map<std::string, std::vector<std::string> > &dirs,
					 int													indent)
{
	std::string pad(indent, ' ');

	for (std::map<std::string, std::vector<std::string> >::const_iterator it =
			 dirs.begin();
		 it != dirs.end();
		 ++it)
	{
		std::cout << pad << it->first << " : ";

		for (size_t i = 0; i < it->second.size(); ++i)
			std::cout << it->second[i] << " ";

		std::cout << std::endl;
	}
}

void printHttpConfig(const HttpConfig &config)
{
	std::cout << "\n========== HTTP ==========\n";

	printDirectives(config.directives, 2);

	for (size_t i = 0; i < config.servers.size(); ++i)
	{
		std::cout << "\n  ===== SERVER " << i << " =====\n";

		printDirectives(config.servers[i].directives, 4);

		for (size_t j = 0; j < config.servers[i].locations.size(); ++j)
		{
			const LocationConfig &loc = config.servers[i].locations[j];

			std::cout << "\n    --- LOCATION: "
					  << loc.path << " ---\n";

			printDirectives(loc.directives, 6);
		}
	}
}
// clang-format on


int main()
{
	try
	{
		// Lexer lexer("config/config_nginx.txt");
		// lexer.parseConfigFile("config/test.txt");

		std::ifstream file("config/default.txt");

		if (!file.is_open())
		{
			std::cerr << "Error: cannot open config file" << std::endl;
			return 1;
		}

		ConfigParser parser;
		HttpConfig	 config = parser.parse(file);

		// printHttpConfig(config);
		Logger::log(config);

		std::cout << "Parsed successfully!" << std::endl;
		std::cout << "Servers count: " << config.servers.size() << std::endl;
		// Lexer lexer("config/config_nginx.txt");
		// lexer.parseConfigFile("config/test.txt");

		return 0;
	}
	catch (const std::exception &e)
	{
		Logger::log("Fatal error: " + std::string(e.what()));
	}
}