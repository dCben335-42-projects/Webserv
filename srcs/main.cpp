#include "Lexer.hpp"
#include "config.hpp"
#include "EventLoop.hpp"


void testPrintAddress()
{
	int			a	= 42;
	std::string str = "Hello, World!";
	Logger::log("Address of a: " + LoggerJson::get(a, true));
	Logger::log("Address of str: " + LoggerJson::get(str, true));

	std::vector<int> vec;
	vec.push_back(1);
	vec.push_back(2);
	vec.push_back(3);
	vec.push_back(4);
	vec.push_back(5);
	Logger::log("Address of vec: " + LoggerJson::get(vec));

	std::set<std::string> mySet;
	mySet.insert("apple");
	mySet.insert("banana");
	mySet.insert("cherry");
	Logger::log("Address of mySet: " + LoggerJson::get(mySet, true));

	std::map<std::string, int> myMap;
	myMap["one"]   = 1;
	myMap["two"]   = 2;
	myMap["three"] = 3;
	Logger::log("Address of myMap: " + LoggerJson::get(myMap, true));

	char *str = NULL;

	Logger::log(str == Null ?: "null pointer");
}

int main()
{
	testPrintAddress();
	// try
	// {
	// 	Lexer lex("config/def_language.txt");
	// }
	// catch (const std::exception &e)
	// {
	// 	Logger::log("Fatal error: " + std::string(e.what()));
	// }
}