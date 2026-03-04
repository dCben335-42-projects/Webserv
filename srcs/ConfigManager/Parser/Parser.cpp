#include "Parser.hpp"
#include <stack>
#include "Logger.hpp"
#include "Number.hpp"
#include "String.hpp"
#include "constants.hpp"
#include "webserv.hpp"
#include "log.hpp"

const Lexer::Directive<Config> Lexer::m_serverDirectives[] = {
	{PARSER_KEY_SERVER_NAME, NULL, NULL, &Config::setServerNames, NULL},
	{PARSER_KEY_KEEPALIVE_TIMEOUT, NULL, NULL, NULL, &Config::setKeepAliveTimeout},
	{PARSER_KEY_HEADER_TIMEOUT, NULL, NULL, NULL, &Config::setHeaderTimeout},
	{PARSER_KEY_BODY_TIMEOUT, NULL, NULL, NULL, &Config::setBodyTimeout},
	{PARSER_KEY_ACCESS_LOG, &Config::setAccessLog, NULL, NULL, NULL},
	{PARSER_KEY_ERROR_LOG, &Config::setErrorLog, NULL, NULL, NULL},
	{PARSER_KEY_AUTH_LOCATION, &Config::setAuthLocation, NULL, NULL, NULL},
	{PARSER_KEY_CGI_TIMEOUT, NULL, NULL, NULL, &Config::setCgiTimeout},
	{PARSER_KEY_DEFAULT_TYPE, &Config::setDefaultMime, NULL, NULL, NULL},
	{PARSER_KEY_DEFAULT_CGI_TYPE, &Config::setDefaultCgiMime, NULL, NULL, NULL}};

const Lexer::Directive<Location> Lexer::m_locationDirectives[] = {
	{PARSER_KEY_ROOT, &Location::setRoot, NULL, NULL, NULL},
	{PARSER_KEY_ALIAS, &Location::setAlias, NULL, NULL, NULL},
	{PARSER_KEY_AUTOINDEX, NULL, &Location::setAutoIndex, NULL, NULL},
	{PARSER_KEY_UPLOAD, NULL, NULL, &Location::setUpload, NULL},
	{PARSER_KEY_INDEX, NULL, NULL, &Location::setIndexes, NULL},
	{PARSER_KEY_METHODS, NULL, NULL, &Location::setMethods, NULL},
	{PARSER_KEY_RETURN, NULL, NULL, &Location::setReturn, NULL},
	{PARSER_KEY_MAX_BODY_SIZE, &Location::setMaxBodySize, NULL, NULL, NULL},
	{PARSER_KEY_AUTH_REQUIRED, NULL, &Location::setAuthRequired, NULL, NULL}};

const size_t Lexer::m_serverDirectivesCount	  = sizeof(m_serverDirectives) / sizeof(m_serverDirectives[0]);
const size_t Lexer::m_locationDirectivesCount = sizeof(m_locationDirectives) / sizeof(m_locationDirectives[0]);

Lexer::Lexer(const std::string &fileContent) : m_content(fileContent)
{
}

Lexer::~Lexer()
{
}


/**
 *
 * @brief Retrieves the parsed HTTP configuration.
 *
 * @return A constant reference to the HttpNode representing the HTTP configuration.
 *
 */
const Config &Lexer::getHttpConfig() const
{
	return m_httpConfig;
}

/**
 *
 * @brief Retrieves the parsed server configurations.
 *
 * @return A constant reference to the vector of server configurations.
 *
 */
const VServersConfigs &Lexer::getServersConfigs() const
{
	return m_serversConfigs;
}


/**
 *
 * @brief Saves a token to the tokens vector, handling quoted strings and semicolons.
 *
 * @param tokens The vector of tokens to which the token will be added.
 * @param token The current token being built from the line.
 * @param inQuotes A boolean indicating whether the current token is within quotes.
 *
 * @note If the token is not empty, it is added to the tokens vector.
 * If the token ends with a semicolon and is not within quotes, the semicolon is removed before adding it to the vector.
 *
 */
void Lexer::saveToken(Vec<std::string> &tokens, std::string &token, const bool inQuotes)
{
	if (token.empty())
		return;
	if (!inQuotes && token.at(token.size() - 1) == ';')
	{
		const std::string tokenWithoutSemicolon = token.substr(0, token.size() - 1);
		if (!tokenWithoutSemicolon.empty())
			tokens.push(tokenWithoutSemicolon);
	}
	else
		tokens.push(inQuotes ? String::unescape(token) : token);
	token.clear();
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
Vec<std::string> Lexer::extractTokens(const std::string &line)
{
	Vec<std::string> tokens;
	std::string		 current;
	bool			 inQuotes = false;

	for (std::string::size_type i = 0; i < line.size(); ++i)
	{
		char c = line[i];

		if (c == DQUOTE)
		{
			if (inQuotes && line[i - 1] == '\\')
			{
				current += c;
				continue;
			}
			inQuotes = !inQuotes;
			continue;
		}
		if (std::isspace(static_cast<unsigned char>(c)) && !inQuotes)
			saveToken(tokens, current, inQuotes);
		else
			current += c;
	}
	saveToken(tokens, current, inQuotes);
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
std::string Lexer::joinTokens(const Vec<std::string> &tokens, const size_t start)
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
void Lexer::set(const std::string &key, const Vec<std::string> &tokens, AConfigNode &node, const size_t lineNumber)
{
	if (key == PARSER_KEY_ADDHEADER && tokens.size() >= 2)
		node.setAddHeaders(tokens[0], joinTokens(tokens, 1));
	else if (key == PARSER_KEY_CGI_PASS && !tokens.empty())
		node.setCgiPass(tokens[0], joinTokens(tokens, 1));
	else if (key == PARSER_KEY_ERROR_PAGE && !tokens.empty())
		node.setErrorPages(tokens[0], joinTokens(tokens, 1));
	else
	{
		if (node.hasDirective(key))
			Logger::custom(LOG_MSG_CONFIG_DUPLICATE_DIRECTIVE_START + Logger::get(key) + LOG_MSG_CONFIG_DUPLICATE_DIRECTIVE_END + Logger::get(lineNumber));
		node.setDirective(key, tokens);
	}
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
void Lexer::retreive()
{
	std::istringstream		input(m_content);
	std::string				line;
	std::stack<std::string> context;

	ServerNode	 currentServer;
	LocationNode currentLocation;
	size_t		 lineNumber = 0;

	while (std::getline(input, line))
	{
		++lineNumber;
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
			set(key, tokens, m_http, lineNumber);
		else if (currentContext == PARSER_BLOCK_SERVER)
			set(key, tokens, currentServer, lineNumber);
		else if (currentContext == PARSER_BLOCK_LOCATION)
			set(key, tokens, currentLocation, lineNumber);
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
void Lexer::resolveListen(ServerNode &server)
{
	if (!server.hasDirective(PARSER_KEY_LISTEN))
		return;

	const std::string	&listen	 = server.getDirective(PARSER_KEY_LISTEN).at(0);
	const size_t		 pos	 = listen.rfind(':');
	const std::string	 ip		 = listen.substr(0, pos);
	const std::string	 port	 = listen.substr(pos + 1);
	const unsigned short portNum = Number::toUShort(port);

	if (portNum == 0 || portNum > MAX_PORT_NUMBER)
	{

		Logger::custom(ERROR_MSG_CONFIG_INVALID_PORT + Logger::get(port));
		return;
	}
	server.setSocketAddress(SocketAddress(ip, portNum));
}


/**
 *
 * @brief Checks if a server configuration is a duplicate of any existing server configurations
 * based on socket address and server names.
 *
 * @param server The ServerNode object to be checked for duplication.
 * @param servers The vector of existing ServerNode objects to compare against.
 * @param start The index from which to start checking for duplicates in the servers vector.
 *
 * @return true if a duplicate server configuration is found, false otherwise.
 *
 * @note This function compares the socket address of the given server with those of the existing servers.
 * If a match is found, it further checks the server names to determine
 * if they are considered duplicates based on the presence or absence of server names and their values.
 *
 */
bool Lexer::isDuplicate(
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


/**
 *
 * @brief Fills maps of "any" IP addresses for both IPv4 and IPv6 based on the server configurations.
 *
 * @param ipv4AnyPorts A map to be filled with ports that have an "any" IPv4 address configuration.
 * @param ipv6AnyPorts A map to be filled with ports that have an "any" IPv6 address configuration.
 *
 * @note This function iterates through the server configurations and
 * checks if any of them are configured to listen on an "any" IP address
 * for either IPv4 or IPv6. If it finds such configurations,
 * it adds the corresponding port and "any" SocketAddress to the provided maps.
 *
 */
void Lexer::fillIpAnyMaps(
	Map<unsigned short, SocketAddress> &ipv4AnyPorts,
	Map<unsigned short, SocketAddress> &ipv6AnyPorts) const
{
	const Vec<ServerNode> &servers = m_http.getServers();
	for (size_t i = 0; i < servers.size(); ++i)
	{
		const SocketAddress &addr	= servers[i].getSocketAddress();
		const bool			 isIPv6 = addr.isIPv6();

		if (!isIPv6 && addr.isAnyIPv4())
			ipv4AnyPorts.set(addr.getPort(), SocketAddress::any(false, addr.getPort()));
		else if (isIPv6 && addr.isAnyIPv6())
			ipv6AnyPorts.set(addr.getPort(), SocketAddress::any(true, addr.getPort()));
	}
}


/**
 *
 * @brief Groups server nodes by their socket addresses,
 * taking into account "any" IP address configurations.
 *
 * @return A map where the keys are SocketAddress objects representing the listening addresses and ports,
 *  and the values are vectors of ServerNode objects that share the same listening configuration.
 *
 * @note This function groups server nodes based on their socket addresses.
 * If a server is configured to listen on an "any" IP address for a specific port,
 * it will be grouped under the corresponding "any" SocketAddress in the resulting map.
 *
 */
Lexer::VServersNodes Lexer::groupServersNodes() const
{
	VServersNodes		   grouped;
	const Vec<ServerNode> &servers = m_http.getServers();

	Map<unsigned short, SocketAddress> ipv4AnyPorts;
	Map<unsigned short, SocketAddress> ipv6AnyPorts;
	fillIpAnyMaps(ipv4AnyPorts, ipv6AnyPorts);

	for (size_t i = 0; i < servers.size(); ++i)
	{
		const ServerNode	&server = servers[i];
		const SocketAddress &addr	= server.getSocketAddress();
		const bool			 isIPv6 = addr.isIPv6();
		SocketAddress		 key	= addr;

		if (isIPv6 && ipv6AnyPorts.has(addr.getPort()))
			key = ipv6AnyPorts.get(addr.getPort());
		else if (!isIPv6 && ipv4AnyPorts.has(addr.getPort()))
			key = ipv4AnyPorts.get(addr.getPort());

		if (!grouped.has(key))
			grouped.set(key, VServerNodes());
		grouped.get(key).push(server);
	}
	return grouped;
}


/**
 *
 * @brief Adds a location configuration to a server configuration based on a LocationNode.
 *
 * @param config The Config object representing the server configuration to which the location will be added.
 * @param locIt An iterator pointing to the LocationNode in the map of locations for the server configuration.
 *
 * @note This function creates a Location object based on the directives in the LocationNode,
 * applies the directives to it, and then adds it to the provided Config object under the corresponding path.
 *
 */
void Lexer::addLocation(Config &config, const Map<LocationNode>::const_iterator &locIt)
{
	const LocationNode					&locationNode = locIt->second;
	const AConfigNode::Directives		&directives	  = locationNode.getDirectives();
	AConfigNode::DirectivesConstIt		 dit		  = directives.begin();
	const AConfigNode::DirectivesConstIt end		  = directives.end();

	Location location;
	while (dit != end)
	{
		if (!applyDirective(location, dit->first, dit->second, m_locationDirectives, m_locationDirectivesCount))
		{
			if (dit->first != PARSER_KEY_LISTEN)
				Logger::custom(ERROR_MSG_CONFIG_PREFIX LOG_MSG_UNKNOWN_DIRECTIVE + Logger::get(dit->first) + LOG_MSG_WITH_VALUE + Logger::get(dit->second));
		}
		++dit;
	}
	location.setAddHeaders(locationNode.getAddHeaders());
	location.setErrorPages(locationNode.getErrorPages());
	location.setCgiBin(locationNode.getCgiPass());
	location.setPath(locIt->first);
	config.setLocation(locIt->first, location);
}


/**
 *
 * @brief Adds a server configuration to the list of server configurations for a given socket address.
 *
 * @param configs The list of server configurations to which the new configuration will be added.
 * @param serverNode The ServerNode object containing the directives and locations for the server configuration.
 *
 * @note This function creates a Config object based on the directives in the ServerNode,
 * applies the directives to it, and then adds it to the provided list of server configurations.
 * It also processes any location blocks within the server node
 * and adds them to the Config object before adding it to the list.
 *
 */
void Lexer::addConfig(ServerConfigs &configs, const ServerNode &serverNode)
{
	Config serverConfig(m_httpConfig);
	serverConfig.setAddress(serverNode.getSocketAddress());
	Location defaultLocation(m_httpConfig.getDefaultLocation());

	const AConfigNode::Directives		&directives = serverNode.getDirectives();
	AConfigNode::DirectivesConstIt		 dit		= directives.begin();
	const AConfigNode::DirectivesConstIt end		= directives.end();

	while (dit != end)
	{
		if (!applyDirective(serverConfig, dit->first, dit->second, m_serverDirectives, m_serverDirectivesCount))
		{
			if (!applyDirective(defaultLocation, dit->first, dit->second, m_locationDirectives, m_locationDirectivesCount))
			{
				if (dit->first != PARSER_KEY_LISTEN)
					Logger::custom(ERROR_MSG_CONFIG_PREFIX LOG_MSG_UNKNOWN_DIRECTIVE + Logger::get(dit->first) + LOG_MSG_WITH_VALUE + Logger::get(dit->second));
			}
		}
		++dit;
	}
	defaultLocation.combineAddHeaders(serverNode.getAddHeaders());
	defaultLocation.combineErrorPages(serverNode.getErrorPages());
	defaultLocation.combineCgiBin(serverNode.getCgiPass());
	serverConfig.setDefaultLocation(defaultLocation);

	const Map<LocationNode> &locations = serverNode.getLocations();
	for (Map<LocationNode>::const_iterator locIt = locations.begin(); locIt != locations.end(); ++locIt)
		addLocation(serverConfig, locIt);
	checkTimeoutValues(serverConfig);
	configs.push(serverConfig);
}



/**
 *
 * @brief Creates the HTTP configuration based on the directives in the HttpNode.pCOnfig
 *
 * @note This function iterates through the directives in the HttpNode
 * and applies them to the m_httpConfig object.
 * It also handles any directives that are applicable to locations
 * and applies them to a default location configuration.
 * Finally, it sets the default location in the m_httpConfig object.
 *
 */
void Lexer::createHttpConfig()
{
	const AConfigNode::Directives		&directives = m_http.getDirectives();
	AConfigNode::DirectivesConstIt		 dit		= directives.begin();
	const AConfigNode::DirectivesConstIt end		= directives.end();

	Location defaultLocation;
	defaultLocation.setMaxBodySize(FALLBACK_MAX_BODY_SIZE);
	while (dit != end)
	{
		if (dit->first == PARSER_KEY_LISTEN)
			continue;
		if (!applyDirective(m_httpConfig, dit->first, dit->second, m_serverDirectives, m_serverDirectivesCount))
		{
			if (!applyDirective(defaultLocation, dit->first, dit->second, m_locationDirectives, m_locationDirectivesCount))
			{
				Logger::custom(ERROR_MSG_CONFIG_PREFIX LOG_MSG_UNKNOWN_DIRECTIVE + Logger::get(dit->first) + LOG_MSG_WITH_VALUE + Logger::get(dit->second));
			}
		}
		++dit;
	}
	defaultLocation.setAddHeaders(m_http.getAddHeaders());
	defaultLocation.setErrorPages(m_http.getErrorPages());
	defaultLocation.setCgiBin(m_http.getCgiPass());
	m_httpConfig.setDefaultLocation(defaultLocation);
	checkTimeoutValues(m_httpConfig);
}


/**
 *
 * @brief Creates the server configurations based on the grouped server nodes.
 *
 * @note This function groups the server nodes by their socket addresses and then creates a list of server configurations for each unique socket address.
 * It iterates through the grouped server nodes, applies the directives to create Config objects, and adds them to the m_serversConfigs map with the corresponding socket address as the key.
 *
 */
void Lexer::createServersConfigs()
{
	const VServersNodes groupedServers = groupServersNodes();
	for (VServersNodes::const_iterator it = groupedServers.begin(); it != groupedServers.end(); ++it)
	{
		const SocketAddress &socketAddress = it->first;
		const VServerNodes	&serverNodes   = it->second;

		ServerConfigs serverConfigs;
		for (size_t i = 0; i < serverNodes.size(); ++i)
			addConfig(serverConfigs, serverNodes[i]);
		m_serversConfigs.set(socketAddress, serverConfigs);
	}
}


/**
 *
 * @brief Creates the HTTP and server configurations based on the parsed configuration data.
 *
 * @note This function first creates the HTTP configuration by applying the directives from the HttpNode,
 * and then creates the server configurations by grouping the server nodes and applying their directives to create Config objects.
 *
 */
void Lexer::createConfigs()
{
	createHttpConfig();
	createServersConfigs();
}



/**
 *
 * @brief Parses the configuration content and creates the HTTP and server configurations.
 *
 * @note This function first retrieves the configuration data by parsing the content,
 * and then creates the HTTP and server configurations based on the parsed data.
 *
 */
void Lexer::checkTimeoutValues(Config &config)
{
	time_t		 keepAliveTimeOut = config.getKeepAliveTimeout();
	time_t		 cgiTimeOut		  = config.getCgiTimeout();
	const time_t headerTimeOut	  = config.getHeaderTimeout();
	const time_t bodyTimeOut	  = config.getBodyTimeout();

	if (headerTimeOut < MIN_HEADER_TIMEOUT)
	{
		Logger::custom(ERROR_MSG_CONFIG_PREFIX PARSER_KEY_HEADER_TIMEOUT LOG_MSG_IS_SET_TO + Logger::get(headerTimeOut) + LOG_MSG_LESS_THAN_RECO + Logger::get(MIN_HEADER_TIMEOUT) + LOG_MSG_SECONDS_END_MSG LOG_GLOBAL_SEPARATOR +
					   LOG_MSG_CONSIDER PARSER_KEY_HEADER_TIMEOUT LOG_MSG_BAD_HEADERS_TO
						   LOG_GLOBAL_SEPARATOR	 LOG_MSG_RESETTING +
					   PARSER_KEY_HEADER_TIMEOUT LOG_KEYWORD_TO + Logger::get(MIN_HEADER_TIMEOUT) + LOG_MSG_SECONDS_END_MSG);
		config.setHeaderTimeout(MIN_HEADER_TIMEOUT);
	}
	if (bodyTimeOut < MIN_BODY_TIMEOUT)
	{
		Logger::custom(ERROR_MSG_CONFIG_PREFIX PARSER_KEY_BODY_TIMEOUT LOG_MSG_IS_SET_TO + Logger::get(bodyTimeOut) + LOG_MSG_LESS_THAN_RECO + Logger::get(MIN_BODY_TIMEOUT) + LOG_MSG_SECONDS_END_MSG LOG_GLOBAL_SEPARATOR +
					   LOG_MSG_CONSIDER + PARSER_KEY_BODY_TIMEOUT + LOG_MSG_BAD_BODY_TO LOG_GLOBAL_SEPARATOR LOG_MSG_RESETTING + PARSER_KEY_BODY_TIMEOUT LOG_KEYWORD_TO + Logger::get(MIN_BODY_TIMEOUT) +
					   LOG_MSG_SECONDS_END_MSG);
		config.setBodyTimeout(MIN_BODY_TIMEOUT);
	}
	if (keepAliveTimeOut < MIN_KEEP_ALIVE_TIMEOUT)
	{
		Logger::custom(ERROR_MSG_CONFIG_PREFIX PARSER_KEY_KEEPALIVE_TIMEOUT LOG_MSG_IS_SET_TO + Logger::get(keepAliveTimeOut) + LOG_MSG_LESS_THAN_RECO + Logger::get(MIN_KEEP_ALIVE_TIMEOUT) + LOG_MSG_SECONDS_END_MSG LOG_GLOBAL_SEPARATOR +
					   LOG_MSG_CONSIDER PARSER_KEY_KEEPALIVE_TIMEOUT LOG_MSG_BAD_KEEPALIVE_TO LOG_GLOBAL_SEPARATOR LOG_MSG_RESETTING + PARSER_KEY_KEEPALIVE_TIMEOUT LOG_KEYWORD_TO + Logger::get(MIN_KEEP_ALIVE_TIMEOUT) +
					   LOG_MSG_SECONDS_END_MSG);
		config.setKeepAliveTimeout(MIN_KEEP_ALIVE_TIMEOUT);
	}
	if (cgiTimeOut < MIN_CGI_TIMEOUT)
	{
		Logger::custom(ERROR_MSG_CONFIG_PREFIX PARSER_KEY_CGI_TIMEOUT LOG_MSG_IS_SET_TO + Logger::get(cgiTimeOut) + LOG_MSG_LESS_THAN_RECO + Logger::get(MIN_CGI_TIMEOUT) + LOG_MSG_SECONDS_END_MSG LOG_GLOBAL_SEPARATOR +
					   LOG_MSG_CONSIDER + PARSER_KEY_CGI_TIMEOUT + LOG_MSG_BAD_CGI_TO + LOG_GLOBAL_SEPARATOR LOG_MSG_RESETTING + PARSER_KEY_CGI_TIMEOUT LOG_KEYWORD_TO + Logger::get(MIN_CGI_TIMEOUT) +
					   LOG_MSG_SECONDS_END_MSG);
		config.setCgiTimeout(MIN_CGI_TIMEOUT);
	}


	keepAliveTimeOut  = config.getKeepAliveTimeout();
	cgiTimeOut		  = config.getCgiTimeout();
	const time_t diff = keepAliveTimeOut - cgiTimeOut;
	if (diff < 0)
	{
		Logger::custom(ERROR_MSG_CONFIG_PREFIX PARSER_KEY_CGI_TIMEOUT LOG_MSG_IS_GREATER PARSER_KEY_KEEPALIVE_TIMEOUT LOG_KEYWORD_BY + Logger::get(-diff) + LOG_MSG_SECONDS_END_MSG LOG_GLOBAL_SEPARATOR LOG_MSG_CONNECTION_TO_BEFORE_CGI +
					   LOG_MSG_RESETTING PARSER_KEY_CGI_TIMEOUT																																			 LOG_KEYWORD_TO PARSER_KEY_KEEPALIVE_TIMEOUT " - " + Logger::get(MIN_CGI_DIFF_TIMEOUT) + LOG_MSG_SECONDS_END_MSG);

		config.setCgiTimeout(keepAliveTimeOut - MIN_CGI_DIFF_TIMEOUT);
	}
	else if (diff < MIN_CGI_DIFF_TIMEOUT)
	{
		Logger::custom(ERROR_MSG_CONFIG_PREFIX PARSER_KEY_CGI_TIMEOUT LOG_MSG_IS_LESS PARSER_KEY_KEEPALIVE_TIMEOUT LOG_KEYWORD_BY_ONLY + Logger::get(diff) + LOG_MSG_SECONDS_END_MSG LOG_GLOBAL_SEPARATOR LOG_MSG_CGI_TO_BEFORE_CONNECTON +
					   LOG_MSG_RESETTING PARSER_KEY_CGI_TIMEOUT																																			  LOG_KEYWORD_TO PARSER_KEY_KEEPALIVE_TIMEOUT " - " + Logger::get(MIN_CGI_DIFF_TIMEOUT) + LOG_MSG_SECONDS_END_MSG);
		config.setCgiTimeout(keepAliveTimeOut - MIN_CGI_DIFF_TIMEOUT);
	}
}


/**
 *
 * @brief Resolves the "listen" directive for all server configurations, ensuring that socket addresses are properly set.
 *
 * @note This function iterates through all server nodes in the HttpNode configuration
 * and calls resolveListen on each one to ensure that
 * the socket addresses are correctly set based on the "listen" directive.
 *
 */
void Lexer::resolve()
{
	Vec<ServerNode> &servers	= m_http.getServers();
	size_t			 numServers = servers.size();

	for (size_t i = 0; i < numServers; ++i)
		resolveListen(servers[i]);
}


/**
 *
 * @brief Removes duplicate server configurations based on their socket addresses and server names.
 *
 * @note It Logs warnings when duplicates are found or when a server configuration is missing a "listen" directive.
 *
 */
void Lexer::removeDuplicate()
{
	Vec<ServerNode> &servers = m_http.getServers();
	size_t			 i		 = 0;
	while (i < servers.size())
	{
		if (!servers[i].hasDirective(PARSER_KEY_LISTEN))
		{
			Logger::custom(ERROR_MSG_NO_LISTEN_DIRECTIVE + Logger::get(servers[i]));
			servers.remove(i);
			continue;
		}
		if (!servers[i].getSocketAddress().getLength())
		{
			Logger::custom(ERROR_MSG_INVALID_LISTEN_DIRECTIVE + Logger::get(servers[i]));
			servers.remove(i);
			continue;
		}
		if (isDuplicate(servers[i], servers, i + 1))
		{
			Logger::custom(ERROR_MSG_DUPLICATE_LISTEN_DIRECTIVE + Logger::get(servers[i].getSocketAddress()) + ERROR_MSG_SERVER_IGNORED + Logger::get(servers[i]));
			servers.remove(i);
			continue;
		}
		++i;
	}
}