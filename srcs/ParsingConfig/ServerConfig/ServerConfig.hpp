#ifndef CONFIG_SERVER_HPP
#define CONFIG_SERVER_HPP

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <stack>
#include <fstream>
#include <iostream>
#include <cctype>
#include "Logger.hpp"
#include "LocationConfig.hpp"

struct ServerConfig
{
		// clang-format off
		std::map<std::string, std::vector<std::string> > directives;
		std::map<std::string, LocationConfig>			locations; // clé = path

		std::map<std::string, std::string> add_headers;
		std::map<std::string, std::string> cgi_pass;
		std::map<std::string, std::string> error_pages;
};

std::ostream &operator<<(std::ostream &os, const ServerConfig &data);

#endif