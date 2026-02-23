#include "Parser.hpp"
#include <stack>
#include "Logger.hpp"


#define DQUOTE '"'
#define SQUOTE '\''
#define SPACE ' '

#define PARSER_BLOCK_START "{"
#define PARSER_BLOCK_END "}"

#define PARSER_BLOCK_HTTP "http"
#define PARSER_BLOCK_SERVER "server"
#define PARSER_BLOCK_LOCATION "location"

#define PARSER_KEY_ADDHEADER "add_header"
#define PARSER_KEY_CGI_PASS "cgi_pass"
#define PARSER_KEY_ERROR_PAGE "error_page"

#define PARSER_KEY_LISTEN "listen"
#define PARSER_KEY_HOST "host"
#define PARSER_KEY_PORT "port"

Parser::Parser(const std::string &fileContent) : m_content(fileContent)
{
}

Parser::~Parser()
{
}


const HttpConfig &Parser::getHttpConfig() const
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
 * @brief Parses the configuration content and populates the HttpConfig object.
 *
 * @note Updates m_http member.
 *
 * @note This function reads the configuration content line by line,
 * tokenizes it, and builds the configuration hierarchy based on the context
 * (http, server, location).
 *
 * It uses a stack to keep track of the current context and
 * updates the HttpConfig, ServerConfig, and LocationConfig objects accordingly.
 *
 */
void Parser::retreive()
{
	std::istringstream		input(m_content);
	std::string				line;
	std::stack<std::string> context;

	ServerConfig   currentServer;
	LocationConfig currentLocation;

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
			currentServer = ServerConfig();
			continue;
		}
		if (key == PARSER_BLOCK_LOCATION)
		{
			context.push(PARSER_BLOCK_LOCATION);
			currentLocation = LocationConfig();
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
 * @param server The ServerConfig object for which to resolve the "listen" directive.
 *
 * @note This function checks if the "listen" directive is present in the server configuration.
 * If it exists, it extracts the host and port from the directive value and sets them in the ServerConfig object.
 *
 */
void Parser::resolveListen(ServerConfig &server)
{
	if (!server.hasDirective(PARSER_KEY_LISTEN))
		return;

	const std::string &listen = server.getDirective(PARSER_KEY_LISTEN).at(0);
	const size_t	   pos	  = listen.rfind(':');
	const std::string  ip	  = listen.substr(0, pos);
	bool			   isIPv6 = ip.front() == '[' && ip.back() == ']';

	const std::string port = listen.substr(pos + 1);
	server.setIp(ip);
	server.setPort(port);
}


void Parser::parse()
{
	Vec<ServerConfig> servers	 = m_http.getServers();
	const size_t	  numServers = servers.size();

	for (size_t i = 0; i < numServers; ++i)
		resolveListen(servers[i]);
}