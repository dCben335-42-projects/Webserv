#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include "HttpNode.hpp"

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

#define PARSER_KEY_SERVER_NAME "server_name"
#define PARSER_KEY_ROOT "root"
#define PARSER_KEY_ALIAS "alias"
#define PARSER_KEY_INDEX "index"
#define PARSER_KEY_AUTOINDEX "auto_index"
#define PARSER_KEY_UPLOAD "sendfiles"
#define PARSER_KEY_RETURN "return"
#define PARSER_KEY_METHODS "allow_methods"
#define PARSER_KEY_KEEPALIVE_TIMEOUT "keepalive_timeout"
#define PARSER_KEY_HEADER_TIMEOUT "header_timeout"
#define PARSER_KEY_BODY_TIMEOUT "body_timeout"
#define PARSER_KEY_MAX_BODY_SIZE "client_max_body_size"
#define PARSER_KEY_CGI_PARAMS "cgi_params"

#include "Config.hpp"

typedef Vec<Config> ServerConfigs;

class Parser
{
	private:
		struct ServerDirective;
		struct LocationDirective;

	private:
		HttpNode		   m_http;
		const std::string &m_content;
		Config			   m_httpConfig;
		Vec<ServerConfigs> m_serversConfigs;

		static const LocationDirective m_locationDirectives[];
		static const ServerDirective   m_serverDirectives[];

		Vec<std::string> extractTokens(const std::string &line);
		std::string		 joinTokens(const Vec<std::string> &tokens, const size_t start);

		void set(const std::string &key, const Vec<std::string> &tokens, AConfigNode &node);
		void resolveListen(ServerNode &server);

		bool isDuplicate(const ServerNode &server, const Vec<ServerNode> &servers, const size_t start = 0);
		void removeDuplicateSocketAddresses(Vec<ServerNode> &servers);


		bool applyDirective(Config &config, const std::string &key, const Vec<std::string> &values);
		bool applyDirective(Location &location, const std::string &key, const Vec<std::string> &values);

		bool applyStringDirective(Config &config, const ServerDirective &directive, const std::string &value);
		bool applyBoolDirective(Config &config, const ServerDirective &directive, const std::string &value);
		bool applyArrayDirective(Config &config, const ServerDirective &directive, const Vec<std::string> &values);
		bool applyNumberDirective(Config &config, const ServerDirective &directive, const std::string &value);

		bool applyStringDirective(Location &location, const LocationDirective &directive, const std::string &value);
		bool applyBoolDirective(Location &location, const LocationDirective &directive, const std::string &value);
		bool applyArrayDirective(Location &location, const LocationDirective &directive, const Vec<std::string> &values);
		bool applyNumberDirective(Location &location, const LocationDirective &directive, const std::string &value);

	public:
		Parser(const std::string &fileContent);
		~Parser();

		const HttpNode &getHttpConfig() const;

		void retreive();
		void parse();

	private:
		enum DirectiveType
		{
			STRING,
			BOOL,
			ARRAY,
			NUMBER
		};
		struct ServerDirective
		{
				const char		   *key;
				const DirectiveType type;
				void (Config::*setString)(const std::string &);
				void (Config::*setBool)(bool);
				void (Config::*setArray)(const Vec<std::string> &);
				void (Config::*setNumber)(unsigned int);
		};
		struct LocationDirective
		{
				const char		   *key;
				const DirectiveType type;
				void (Location::*setString)(const std::string &);
				void (Location::*setBool)(bool);
				void (Location::*setArray)(const Vec<std::string> &);
				void (Location::*setNumber)(unsigned int);
		};
};

#endif
