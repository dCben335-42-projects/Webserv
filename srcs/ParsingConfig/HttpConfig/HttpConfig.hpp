#ifndef CONFIG_HTTP_HPP
#define CONFIG_HTTP_HPP

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <stack>
#include <fstream>
#include <iostream>
#include <cctype>
#include "Logger.hpp"
#include "ServerConfig.hpp"

struct HttpConfig
{
		// clang-format off
		std::map<std::string, std::vector<std::string> > directives;
		std::vector<ServerConfig>						servers;

		std::map<std::string, std::string> add_headers;
		std::map<std::string, std::string> cgi_pass;
		std::map<std::string, std::string> error_pages;
};

std::ostream &operator<<(std::ostream &os, const HttpConfig &data);

#endif