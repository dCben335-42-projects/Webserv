// #ifndef CONFIG_PARSER_HPP
// #define CONFIG_PARSER_HPP

// #include <string>
// #include <map>
// #include <vector>
// #include <sstream>
// #include <stack>
// #include <fstream>
// #include <iostream>
// #include "Logger.hpp"

// struct LocationConfig
// {
// 		std::string path;
// 		// clang-format off
// 		std::map<std::string, std::vector<std::string> > directives;
// 		std::map<std::string, std::string> add_headers;
// };

// std::ostream &operator<<(std::ostream &os, const LocationConfig &data)
// {
// 	Logger::incrementIndent();
// 	os << Logger::get(data.path) << ": ";
// 	os << Logger::get(data.directives);
// 	Logger::decrementIndent();
// 	return os;
// }

// struct ServerConfig
// {
// 		// clang-format off
// 		std::map<std::string, std::vector<std::string> > directives;
// 		std::vector<LocationConfig>						locations;
// 		std::map<std::string, std::string> add_headers;
// };

// std::ostream &operator<<(std::ostream &os, const ServerConfig &data)
// {
// 	os << Logger::get(data.directives) << "\n";
// 	os << Logger::indent() << "locations: " << Logger::get(data.locations) << "\n";
// 	os << Logger::get(data.add_headers) << "\n";
// 	return os;
// }

// struct HttpConfig
// {
// 		// clang-format off
// 		std::map<std::string, std::vector<std::string> > directives;
// 		std::vector<ServerConfig>						servers;
// 		std::map<std::string, std::string> add_headers;

// };

// std::ostream &operator<<(std::ostream &os, const HttpConfig &data)
// {
// 	os << "HttpConfig:\n";
// 	os << Logger::get(data.directives) << "\n";
// 	os << Logger::indent() << "servers: " << Logger::get(data.servers) << "\n";
// 	os << Logger::get(data.add_headers) << "\n";
// 	return os;
// }

// std::vector<std::string> split(const std::string &line)
// {
//     std::vector<std::string> tokens;
//     std::string current;
//     bool inQuotes = false;

//     for (size_t i = 0; i < line.size(); ++i)
//     {
//         char c = line[i];

//         if (c == '"')
//         {
//             inQuotes = !inQuotes;
//             continue; // on ne garde pas les "
//         }

//         if (isspace(c) && !inQuotes)
//         {
//             if (!current.empty())
//             {
//                 tokens.push_back(current);
//                 current.clear();
//             }
//         }
//         else
//         {
//             current += c;
//         }
//     }

//     if (!current.empty())
//         tokens.push_back(current);

//     return tokens;
// }

// class ConfigParser
// {
// private:
//     HttpConfig http;

//     std::string joinTokens(const std::vector<std::string> &tokens, size_t start)
//     {
//         std::string result;

//         for (size_t i = start; i < tokens.size(); ++i)
//         {
//             if (i > start)
//                 result += " ";
//             result += tokens[i];
//         }
//         return result;
//     }

// public:
//     HttpConfig parse(std::istream &input)
//     {
//         std::string line;
//         std::stack<std::string> context;

//         ServerConfig currentServer;
//         LocationConfig currentLocation;

//         while (std::getline(input, line))
//         {
//             if (line.empty())
//                 continue;

//             std::vector<std::string> tokens = split(line);
//             if (tokens.empty())
//                 continue;

//             ////////////////////////////////////////////////////////////
//             // OPEN BLOCKS
//             ////////////////////////////////////////////////////////////

//             if (tokens[0] == "http")
//             {
//                 context.push("http");
//                 continue;
//             }

//             if (tokens[0] == "server")
//             {
//                 context.push("server");
//                 currentServer = ServerConfig();
//                 continue;
//             }

//             if (tokens[0] == "location")
//             {
//                 context.push("location");
//                 currentLocation = LocationConfig();

//                 if (tokens.size() > 1)
//                     currentLocation.path = tokens[1];

//                 continue;
//             }

//             ////////////////////////////////////////////////////////////
//             // CLOSE BLOCK
//             ////////////////////////////////////////////////////////////

//             if (tokens[0] == "}")
//             {
//                 if (context.top() == "location")
//                     currentServer.locations.push_back(currentLocation);
//                 else if (context.top() == "server")
//                     http.servers.push_back(currentServer);

//                 context.pop();
//                 continue;
//             }

//             ////////////////////////////////////////////////////////////
//             // DIRECTIVE
//             ////////////////////////////////////////////////////////////

//             std::string key = tokens[0];
//             tokens.erase(tokens.begin());

//             ////////////////////////////////////////////////////////////
//             // ADD_HEADER SPECIAL CASE
//             ////////////////////////////////////////////////////////////

//             if (key == "add_header" && tokens.size() >= 2)
//             {
//                 std::string headerName = tokens[0];
//                 std::string value = joinTokens(tokens, 1);

//                 if (context.top() == "http")
//                     http.add_headers[headerName] = value;
//                 else if (context.top() == "server")
//                     currentServer.add_headers[headerName] = value;
//                 else if (context.top() == "location")
//                     currentLocation.add_headers[headerName] = value;

//                 continue;
//             }

//             ////////////////////////////////////////////////////////////
//             // NORMAL DIRECTIVE
//             ////////////////////////////////////////////////////////////

//             if (context.top() == "http")
//                 http.directives[key] = tokens;
//             else if (context.top() == "server")
//                 currentServer.directives[key] = tokens;
//             else if (context.top() == "location")
//                 currentLocation.directives[key] = tokens;
//         }

//         return http;
//     }
// };

// #endif


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

////////////////////////////////////////////////////////////
// STRUCTS
////////////////////////////////////////////////////////////

// clang-format off
struct LocationConfig
{
		std::string path;

		std::map<std::string, std::vector<std::string> > directives;

		std::map<std::string, std::string> add_headers;
		std::map<std::string, std::string> cgi_pass;
		std::map<std::string, std::string> error_pages;
};

std::ostream &operator<<(std::ostream &os, const LocationConfig &data)
{
	Logger::incrementIndent();
	os << Logger::get(data.path) << ": ";
	os << Logger::get(data.directives) << "\n";
	if (!data.add_headers.empty())
		os << Logger::indent() << "add_headers: " << Logger::get(data.add_headers) << "\n";
	if (!data.cgi_pass.empty())
		os << Logger::indent() << "cgi_pass: " << Logger::get(data.cgi_pass) << "\n";
	if (!data.error_pages.empty())
		os << Logger::indent() << "error_pages: " << Logger::get(data.error_pages) << "\n";

	Logger::decrementIndent();
	return os;
}

struct ServerConfig
{
		std::map<std::string, std::vector<std::string> > directives;
		std::vector<LocationConfig>						locations;

		std::map<std::string, std::string> add_headers;
		std::map<std::string, std::string> cgi_pass;
		std::map<std::string, std::string> error_pages;
};

std::ostream &operator<<(std::ostream &os, const ServerConfig &data)
{
	os << Logger::get(data.directives) << "\n";
	os << Logger::indent() << "locations: " << Logger::get(data.locations) << "\n";
	if (!data.add_headers.empty())
		os << Logger::indent() << "add_headers: " << Logger::get(data.add_headers) << "\n";
	if (!data.cgi_pass.empty())
		os << Logger::indent() << "cgi_pass: " << Logger::get(data.cgi_pass) << "\n";
	if (!data.error_pages.empty())
		os << Logger::indent() << "error_pages: " << Logger::get(data.error_pages) << "\n";
	return os;
}

struct HttpConfig
{
		std::map<std::string, std::vector<std::string> > directives;
		std::vector<ServerConfig>						servers;

		std::map<std::string, std::string> add_headers;
		std::map<std::string, std::string> cgi_pass;
		std::map<std::string, std::string> error_pages;
};

std::ostream &operator<<(std::ostream &os, const HttpConfig &data)
{
	os << "HttpConfig:\n";
	os << Logger::get(data.directives) << "\n";
	os << Logger::indent() << "servers: " << Logger::get(data.servers) << "\n";
	if (!data.add_headers.empty())
		os << Logger::indent() << "add_headers: " << Logger::get(data.add_headers) << "\n";
	if (!data.cgi_pass.empty())
		os << Logger::indent() << "cgi_pass: " << Logger::get(data.cgi_pass) << "\n";
	if (!data.error_pages.empty())
		os << Logger::indent() << "error_pages: " << Logger::get(data.error_pages) << "\n";
	return os;
}

////////////////////////////////////////////////////////////
// SPLIT AVEC SUPPORT QUOTES
////////////////////////////////////////////////////////////

std::vector<std::string> split(const std::string &line)
{
	std::vector<std::string> tokens;
	std::string				 current;
	bool					 inQuotes = false;

	for (size_t i = 0; i < line.size(); ++i)
	{
		char c = line[i];

		if (c == '"')
		{
			inQuotes = !inQuotes;
			continue;
		}

		if (isspace(c) && !inQuotes)
		{
			if (!current.empty())
			{
				tokens.push_back(current);
				current.clear();
			}
		}
		else
		{
			current += c;
		}
	}

	if (!current.empty())
		tokens.push_back(current);

	return tokens;
}

////////////////////////////////////////////////////////////
// PARSER
////////////////////////////////////////////////////////////

class ConfigParser
{
	private:
		HttpConfig http;

		std::string joinTokens(const std::vector<std::string> &tokens, size_t start)
		{
			std::string result;

			for (size_t i = start; i < tokens.size(); ++i)
			{
				if (i > start)
					result += " ";
				result += tokens[i];
			}
			return result;
		}

	public:
		HttpConfig parse(std::istream &input)
		{
			std::string				line;
			std::stack<std::string> context;

			ServerConfig   currentServer;
			LocationConfig currentLocation;

			while (std::getline(input, line))
			{
				if (line.empty())
					continue;

				std::vector<std::string> tokens = split(line);
				if (tokens.empty())
					continue;

				////////////////////////////////////////////////////////////
				// OPEN BLOCKS
				////////////////////////////////////////////////////////////

				if (tokens[0] == "http")
				{
					context.push("http");
					continue;
				}

				if (tokens[0] == "server")
				{
					context.push("server");
					currentServer = ServerConfig();
					continue;
				}

				if (tokens[0] == "location")
				{
					context.push("location");
					currentLocation = LocationConfig();

					if (tokens.size() > 1)
						currentLocation.path = tokens[1];

					continue;
				}

				////////////////////////////////////////////////////////////
				// CLOSE BLOCK
				////////////////////////////////////////////////////////////

				if (tokens[0] == "}")
				{
					if (context.top() == "location")
						currentServer.locations.push_back(currentLocation);
					else if (context.top() == "server")
						http.servers.push_back(currentServer);

					context.pop();
					continue;
				}

				////////////////////////////////////////////////////////////
				// DIRECTIVE
				////////////////////////////////////////////////////////////

				std::string key = tokens[0];
				tokens.erase(tokens.begin());

				////////////////////////////////////////////////////////////
				// ADD_HEADER
				////////////////////////////////////////////////////////////

				if (key == "add_header" && tokens.size() >= 2)
				{
					std::string headerName = tokens[0];
					std::string value	   = joinTokens(tokens, 1);

					if (context.top() == "http")
						http.add_headers[headerName] = value;
					else if (context.top() == "server")
						currentServer.add_headers[headerName] = value;
					else if (context.top() == "location")
						currentLocation.add_headers[headerName] = value;

					continue;
				}

				////////////////////////////////////////////////////////////
				// CGI PASS
				////////////////////////////////////////////////////////////

				if (key == "cgiPass" && tokens.size() >= 1)
				{
					std::string ext	  = tokens[0];
					std::string value = joinTokens(tokens, 1);

					if (context.top() == "http")
						http.cgi_pass[ext] = value;
					else if (context.top() == "server")
						currentServer.cgi_pass[ext] = value;
					else if (context.top() == "location")
						currentLocation.cgi_pass[ext] = value;

					continue;
				}

				////////////////////////////////////////////////////////////
				// ERROR PAGE
				////////////////////////////////////////////////////////////

				if (key == "error_page" && tokens.size() >= 1)
				{
					std::string code  = tokens[0];
					std::string value = joinTokens(tokens, 1);

					if (context.top() == "http")
						http.error_pages[code] = value;
					else if (context.top() == "server")
						currentServer.error_pages[code] = value;
					else if (context.top() == "location")
						currentLocation.error_pages[code] = value;

					continue;
				}

				////////////////////////////////////////////////////////////
				// NORMAL DIRECTIVE
				////////////////////////////////////////////////////////////

				if (context.top() == "http")
					http.directives[key] = tokens;
				else if (context.top() == "server")
					currentServer.directives[key] = tokens;
				else if (context.top() == "location")
					currentLocation.directives[key] = tokens;
			}

			return http;
		}
};

#endif
