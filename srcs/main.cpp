#include "Lexer.hpp"
#include "EventLoop.hpp"
#include "Json.hpp"

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

int main()
{
	try
	{
		Lexer lexer("config/config_nginx.txt");
		lexer.parseConfigFile("config/test.txt");
		return 0;
	}
	catch (const std::exception &e)
	{
		Logger::log("Fatal error: " + std::string(e.what()));
	}
}