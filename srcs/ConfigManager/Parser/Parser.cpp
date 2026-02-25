#include "Parser.hpp"
#include <stack>
#include "Logger.hpp"
#include "Number.hpp"

const Parser::ServerDirective Parser::m_serverDirectives[] = {
	{PARSER_KEY_LISTEN, Parser::STRING, &Config::setIp, NULL, NULL, NULL},
	{PARSER_KEY_SERVER_NAME, Parser::ARRAY, NULL, NULL, &Config::setServerNames, NULL}};

const Parser::LocationDirective Parser::m_locationDirectives[] = {
	{PARSER_KEY_ROOT, Parser::STRING, &Location::setRoot, NULL, NULL, NULL},
	{PARSER_KEY_ALIAS, Parser::STRING, &Location::setAlias, NULL, NULL, NULL},
	{PARSER_KEY_AUTOINDEX, Parser::BOOL, NULL, &Location::setAutoIndex, NULL, NULL},
	{PARSER_KEY_UPLOAD, Parser::STRING, &Location::setUpload, NULL, NULL, NULL},
	{PARSER_KEY_INDEX, Parser::ARRAY, NULL, NULL, &Location::setIndexes, NULL},
	{PARSER_KEY_METHODS, Parser::ARRAY, NULL, NULL, &Location::setMethods, NULL}};

Parser::Parser(const std::string &fileContent) : m_content(fileContent)
{
}

Parser::~Parser()
{
}


const HttpNode &Parser::getHttpConfig() const
{
	return m_http;
}


/**
 *
 * @brief Splits a line into tokens, respecting quoted strings.
 *
 * @param line The line to be split into tokens.
 *
 * @note This function handles quoted strings, allowing spaces within quotes to be treated as part of the same token.
 *
 * @return A vector of tokens extracted from the line.
 *
 */
Vec<std::string> Parser::extractTokens(const std::string &line)
{
	Vec<std::string> tokens;
	std::string		 current;
	bool			 inQuotes = false;

	for (std::string::size_type i = 0; i < line.size(); ++i)
	{
		char c = line[i];

		if (c == DQUOTE)
		{
			inQuotes = !inQuotes;
			continue;
		}
		if (isspace(static_cast<unsigned char>(c)) && !inQuotes)
		{
			if (!current.empty())
			{
				tokens.push(current);
				current.clear();
			}
		}
		else
		{
			current += c;
		}
	}

	if (!current.empty())
		tokens.push(current);

	return tokens;
}


/**
 *
 * @brief Joins tokens into a single string starting from a specified index.
 *
 * @param tokens The vector of tokens to be joined.
 * @param start The index from which to start joining tokens.
 *
 * @return A single string resulting from joining the tokens from the specified index onward, separated by spaces.
 *
 */
std::string Parser::joinTokens(const Vec<std::string> &tokens, const size_t start)
{
	std::string result;
	for (size_t i = start; i < tokens.size(); ++i)
	{
		if (i > start)
			result += SPACE;
		result += tokens[i];
	}
	return result;
}


/**
 *
 * @brief Sets a directive in the given configuration node based on the key and tokens.
 *
 * @param key The directive key to be set.
 * @param tokens The vector of tokens associated with the directive, excluding the key.
 * @param node The configuration node where the directive should be set.
 *
 * @note This function handles specific directives like
 * - "add_header",
 * - "cgi_pass",
 * -  "error_page"
 * with special processing, while other directives are set directly using the setDirective method of the node.
 *
 */
void Parser::set(const std::string &key, const Vec<std::string> &tokens, AConfigNode &node)
{
	if (key == PARSER_KEY_ADDHEADER && tokens.size() >= 2)
		node.setAddHeaders(tokens[0], joinTokens(tokens, 1));
	else if (key == PARSER_KEY_CGI_PASS && !tokens.empty())
		node.setCgiPass(tokens[0], joinTokens(tokens, 1));
	else if (key == PARSER_KEY_ERROR_PAGE && !tokens.empty())
		node.setErrorPages(tokens[0], joinTokens(tokens, 1));
	else
		node.setDirective(key, tokens);
}


/**
 *
 * @brief Parses the configuration content and populates the HttpNode object.
 *
 * @note Updates m_http member.
 *
 * @note This function reads the configuration content line by line,
 * tokenizes it, and builds the configuration hierarchy based on the context
 * (http, server, location).
 *
 * It uses a stack to keep track of the current context and
 * updates the HttpNode, ServerNode, and LocationNode objects accordingly.
 *
 */
void Parser::retreive()
{
	std::istringstream		input(m_content);
	std::string				line;
	std::stack<std::string> context;

	ServerNode	 currentServer;
	LocationNode currentLocation;

	while (std::getline(input, line))
	{
		if (line.empty())
			continue;

		Vec<std::string> tokens = extractTokens(line);
		if (tokens.empty())
			continue;

		const std::string key = tokens[0];

		if (key == PARSER_BLOCK_HTTP)
		{
			context.push(PARSER_BLOCK_HTTP);
			continue;
		}
		if (key == PARSER_BLOCK_SERVER)
		{
			context.push(PARSER_BLOCK_SERVER);
			currentServer = ServerNode();
			continue;
		}
		if (key == PARSER_BLOCK_LOCATION)
		{
			context.push(PARSER_BLOCK_LOCATION);
			currentLocation = LocationNode();
			if (tokens.size() > 1)
				currentLocation.getPath() = tokens[1];
			continue;
		}
		if (key == PARSER_BLOCK_END)
		{
			const std::string &currentContext = context.top();
			if (currentContext == PARSER_BLOCK_LOCATION)
				currentServer.setLocation(currentLocation.getPath(), currentLocation);
			else if (currentContext == PARSER_BLOCK_SERVER)
				m_http.addServer(currentServer);
			context.pop();
			continue;
		}
		tokens.erase(tokens.begin());
		const std::string &currentContext = context.top();
		if (currentContext == PARSER_BLOCK_HTTP)
			set(key, tokens, m_http);
		else if (currentContext == PARSER_BLOCK_SERVER)
			set(key, tokens, currentServer);
		else if (currentContext == PARSER_BLOCK_LOCATION)
			set(key, tokens, currentLocation);
	}
}

/**
 *
 * @brief Resolves the "listen" directive for a server configuration, extracting the IP and port.
 *
 * @param server The ServerNode object for which to resolve the "listen" directive.
 *
 * @note This function checks if the "listen" directive is present in the server configuration.
 * If it exists, it extracts the host and port from the directive value and sets them in the ServerNode object.
 *
 */
void Parser::resolveListen(ServerNode &server)
{
	if (!server.hasDirective(PARSER_KEY_LISTEN))
		return;

	const std::string &listen = server.getDirective(PARSER_KEY_LISTEN).at(0);
	const size_t	   pos	  = listen.rfind(':');
	const std::string  ip	  = listen.substr(0, pos);
	const std::string  port	  = listen.substr(pos + 1);

	server.setSocketAddress(SocketAddress(ip, Number::toUShort(port)));
	Logger::debug("ip: " + ip + " and port: " + port + " resolved as: " + Logger::get(server.getSocketAddress()));
}


bool Parser::isDuplicate(
	const ServerNode	  &server,
	const Vec<ServerNode> &servers,
	const size_t		   start)
{
	const bool				hasServerName = server.hasDirective(PARSER_KEY_SERVER_NAME);
	const Vec<std::string> &serverNames	  = hasServerName ? server.getDirective(PARSER_KEY_SERVER_NAME) : Vec<std::string>();

	for (size_t i = start; i < servers.size(); ++i)
	{
		const ServerNode &current = servers[i];
		if (server.getSocketAddress() == current.getSocketAddress())
		{
			if (!hasServerName)
			{
				if (!current.hasDirective(PARSER_KEY_SERVER_NAME))
					return true;
				continue;
			}
			if (!current.hasDirective(PARSER_KEY_SERVER_NAME))
				continue;
			const Vec<std::string> &currentNames = current.getDirective(PARSER_KEY_SERVER_NAME);
			for (size_t j = 0; j < serverNames.size(); ++j)
			{
				if (currentNames.contains(serverNames[j]))
					return true;
			}
		}
	}
	return false;
}


void Parser::removeDuplicateSocketAddresses(Vec<ServerNode> &servers)
{
	size_t i = 0;
	while (i < servers.size())
	{
		if (isDuplicate(servers[i], servers, i + 1))
		{
			Logger::warn("Duplicate listen directive found for " + Logger::get(servers[i].getSocketAddress()));
			servers.remove(i);
			continue;
		}
		++i;
	}
}


bool Parser::applyStringDirective(Config &config, const ServerDirective &directive, const std::string &value)
{
	(config.*directive.setString)(value);
	return true;
}

bool Parser::applyBoolDirective(Config &config, const ServerDirective &directive, const std::string &value)
{
	if (value == "on" || value == "true" || value == "1")
	{
		(config.*directive.setBool)(true);
		return true;
	}
	else if (value == "off" || value == "false" || value == "0")
	{
		(config.*directive.setBool)(false);
		return true;
	}
	return false;
}

bool Parser::applyArrayDirective(Config &config, const ServerDirective &directive, const Vec<std::string> &values)
{
	(config.*directive.setArray)(values);
	return true;
}

bool Parser::applyNumberDirective(Config &config, const ServerDirective &directive, const std::string &value)
{
	(config.*directive.setNumber)(Number::toSizeT(value));
	return true;
}


bool Parser::applyDirective(Config &config, const std::string &key, const Vec<std::string> &values)
{
	for (size_t i = 0; i < sizeof(m_serverDirectives) / sizeof(ServerDirective); ++i)
	{
		const ServerDirective &directive = m_serverDirectives[i];
		if (key == directive.key)
		{
			if (directive.setString && values.size() == 1)
				return applyStringDirective(config, directive, values[0]);
			if (directive.setBool && values.size() == 1)
				return applyBoolDirective(config, directive, values[0]);
			if (directive.setArray)
				return applyArrayDirective(config, directive, values);
			if (directive.setNumber && values.size() == 1)
				return applyNumberDirective(config, directive, values[0]);
			return true;
		}
	}
	return false;
}

bool Parser::applyStringDirective(Location &location, const LocationDirective &directive, const std::string &value)
{
	(location.*directive.setString)(value);
	return true;
}

bool Parser::applyBoolDirective(Location &location, const LocationDirective &directive, const std::string &value)
{
	if (value == "on" || value == "true" || value == "1")
	{
		(location.*directive.setBool)(true);
		return true;
	}
	else if (value == "off" || value == "false" || value == "0")
	{
		(location.*directive.setBool)(false);
		return true;
	}
	return false;
}

bool Parser::applyArrayDirective(Location &location, const LocationDirective &directive, const Vec<std::string> &values)
{
	(location.*directive.setArray)(values);
	return true;
}

bool Parser::applyNumberDirective(Location &location, const LocationDirective &directive, const std::string &value)
{
	(location.*directive.setNumber)(Number::toSizeT(value));
	return true;
}

bool Parser::applyDirective(Location &location, const std::string &key, const Vec<std::string> &values)
{
	for (size_t i = 0; i < sizeof(m_locationDirectives) / sizeof(LocationDirective); ++i)
	{
		const LocationDirective &directive = m_locationDirectives[i];
		if (key == directive.key)
		{
			if (directive.setString && values.size() == 1)
				return applyStringDirective(location, directive, values[0]);
			if (directive.setBool && values.size() == 1)
				return applyBoolDirective(location, directive, values[0]);
			if (directive.setArray)
				return applyArrayDirective(location, directive, values);
			if (directive.setNumber && values.size() == 1)
				return applyNumberDirective(location, directive, values[0]);
			return true;
		}
	}
	return false;
}


void Parser::parse()
{
	Vec<ServerNode> &servers	= m_http.getServers();
	size_t			 numServers = servers.size();

	for (size_t i = 0; i < numServers; ++i)
		resolveListen(servers[i]);
	removeDuplicateSocketAddresses(servers);
	numServers = servers.size();
	Logger::debug("Number of servers after removing duplicates: " + Logger::get(numServers));
	for (size_t i = 0; i < numServers; i++)
	{
		const ServerNode		&server	   = servers[i];
		const Map<LocationNode> &locations = server.getLocations();
		Config					 serverconf;
		for (Map<LocationNode>::const_iterator it = locations.begin(); it != locations.end(); ++it)
		{
			const LocationNode					&locationNode = it->second;
			const AConfigNode::Directives		&directives	  = locationNode.getDirectives();
			AConfigNode::DirectivesConstIt		 dit		  = directives.begin();
			const AConfigNode::DirectivesConstIt end		  = directives.end();

			Location location;
			while (dit != end)
			{
				applyDirective(location, dit->first, dit->second);
				Logger::debug("Applied directive " + Logger::get(dit->first) + " with value: " + Logger::get(dit->second));
				++dit;
			}
			serverconf.setLocation(it->first, location);
		}

		ServerConfigs serverConfigs;
		serverConfigs.push(serverconf);
		m_serversConfigs.push(serverConfigs);
	}
	Logger::log("servers configs: " + Logger::get(m_serversConfigs));

	Logger::debug("Parsed HTTP config: \n" + Logger::get(m_http));
}