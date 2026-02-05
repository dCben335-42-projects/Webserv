#include "Lexer.hpp"
#include "EventLoop.hpp"
#include "Json.hpp"

void testJson()
{
	JsonObject obj;

	obj.create("name", "John");
	obj.create("age", 30.0);
	obj.create("isStudent", false);

	Vec<AJson *> hobbies;
	hobbies.push(AJson::createString("reading"));
	hobbies.push(AJson::createString("coding"));
	obj.create("hobbies", hobbies);

	Vec<AJson *> address;
	address.push(AJson::createString("street"));
	address.push(AJson::createString("city"));
	obj.create("address", address);

	Logger::log("JSON Object:" + Logger::get(obj));
}

int main()
{
	try
	{
		testJson();
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