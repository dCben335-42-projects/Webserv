#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <stack>
#include <fstream>
#include <iostream>
#include <cctype>
#include "Logger.hpp"
#include "HttpConfig.hpp"

class ConfigParser
{
	private:
		HttpConfig m_http;

	public:
		HttpConfig parse(std::istream &input);
};

#include "ConfigParser.tpp"
#endif
