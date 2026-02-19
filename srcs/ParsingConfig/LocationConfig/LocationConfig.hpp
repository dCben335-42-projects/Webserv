#ifndef CONFIG_LOCATION_HPP
#define CONFIG_LOCATION_HPP

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <stack>
#include <fstream>
#include <iostream>
#include <cctype>
#include "Logger.hpp"

struct LocationConfig
{
		std::string path;
		// clang-format off
		std::map<std::string, std::vector<std::string> > directives;
		std::map<std::string, std::string>				add_headers;
		std::map<std::string, std::string>				cgi_pass;
		std::map<std::string, std::string>				error_pages;
};

std::ostream &operator<<(std::ostream &os, const LocationConfig &data);

#endif