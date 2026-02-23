#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <string>
#include "HttpConfig.hpp"

class ConfigParser
{
	private:
		HttpConfig m_http;

	public:
		HttpConfig parse(std::string &content);
};

#include "ConfigParser.tpp"
#endif
