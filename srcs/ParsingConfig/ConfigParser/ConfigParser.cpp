#include "ConfigParser.hpp"
#include "Map.hpp"
#include "Vec.hpp"
#include <sstream>
#include <stack>
#include <fstream>
#include <iostream>
#include <cctype>

inline Vec<std::string> split(const std::string &line)
{
	Vec<std::string> tokens;
	std::string		 current;
	bool			 inQuotes = false;

	for (std::string::size_type i = 0; i < line.size(); ++i)
	{
		char c = line[i];

		if (c == '"')
		{
			inQuotes = !inQuotes;
			continue;
		}

		if (isspace(static_cast<unsigned char>(c)) && !inQuotes)
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

std::string joinTokens(const Vec<std::string> &tokens, size_t start)
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

#include <sstream>

HttpConfig ConfigParser::parse(std::string &content)
{
	std::istringstream		input(content);
	std::string				line;
	std::stack<std::string> context;

	ServerConfig   currentServer;
	LocationConfig currentLocation;

	while (std::getline(input, line))
	{
		if (line.empty())
			continue;

		Vec<std::string> tokens = split(line);
		if (tokens.empty())
			continue;

		std::string key = tokens[0];

		// --- OPEN BLOCK ---
		if (key == "http")
		{
			context.push("http");
			continue;
		}
		if (key == "server")
		{
			context.push("server");
			currentServer = ServerConfig();
			continue;
		}
		if (key == "location")
		{
			context.push("location");
			currentLocation = LocationConfig();
			if (tokens.size() > 1)
				currentLocation.getPath() = tokens[1];
			continue;
		}

		// --- CLOSE BLOCK ---
		if (key == "}")
		{
			if (context.top() == "location")
			{
				currentServer.getLocations()[currentLocation.getPath()] = currentLocation;
			}
			else if (context.top() == "server")
			{
				m_http.getServers().push_back(currentServer);
			}
			context.pop();
			continue;
		}

		// --- DIRECTIVE ---
		tokens.erase(tokens.begin());

		if (context.top() == "http")
		{
			setDirective(m_http, key, tokens);
		}
		else if (context.top() == "server")
		{
			setDirective(currentServer, key, tokens);
		}
		else if (context.top() == "location")
		{
			setDirective(currentLocation, key, tokens);
		}
	}

	return m_http;
}
