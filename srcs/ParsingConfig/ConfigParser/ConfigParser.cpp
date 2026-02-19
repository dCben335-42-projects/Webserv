#include "ConfigParser.hpp"

inline std::vector<std::string> split(const std::string &line)
{
	std::vector<std::string> tokens;
	std::string				 current;
	bool					 inQuotes = false;

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

HttpConfig ConfigParser::parse(std::istream &input)
{
	std::string				line;
	std::stack<std::string> context;

	ServerConfig   currentServer;
	LocationConfig currentLocation;

	while (std::getline(input, line))
	{
		Logger::log("Parsing line: " + line);
		if (line.empty())
			continue;

		std::vector<std::string> tokens = split(line);
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
				currentLocation.path = tokens[1];
			continue;
		}

		// --- CLOSE BLOCK ---
		if (key == "}")
		{
			if (context.top() == "location")
			{
				currentServer.locations[currentLocation.path] = currentLocation;
			}
			else if (context.top() == "server")
			{
				m_http.servers.push_back(currentServer);
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
