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
#define PARSER_KEY_AUTOINDEX "autoindex"
#define PARSER_KEY_UPLOAD "sendfiles"
#define PARSER_KEY_RETURN "return"
#define PARSER_KEY_METHODS "allow_methods"
#define PARSER_KEY_KEEPALIVE_TIMEOUT "keepalive_timeout"
#define PARSER_KEY_HEADER_TIMEOUT "header_timeout"
#define PARSER_KEY_BODY_TIMEOUT "body_timeout"
#define PARSER_KEY_MAX_BODY_SIZE "client_max_body_size"
#define PARSER_KEY_CGI_PARAMS "cgi_params"

#define MAX_PORT_NUMBER 65535


#include "Config.hpp"

typedef Vec<Config> ServerConfigs;

class Parser
{
	public:
		typedef Map<SocketAddress, ServerConfigs> VServerConfigs;
		typedef VServerConfigs::const_iterator	  VServerConfCIt;

	private:
		template <typename T>
		struct Directive
		{
				const std::string key;
				void (T::*setString)(const std::string &);
				void (T::*setBool)(bool);
				void (T::*setArray)(const Vec<std::string> &);
				void (T::*setNumber)(unsigned int);
		};

	private:
		typedef Vec<ServerNode> VServerNodes;

		typedef Map<SocketAddress, VServerNodes> VServersNodes;
		typedef VServersNodes::const_iterator	 VServNodesCIt;

		HttpNode		   m_http;
		const std::string &m_content;
		Config			   m_httpConfig;
		VServerConfigs	   m_serversConfigs;

		static const Directive<Location> m_locationDirectives[];
		static const Directive<Config>	 m_serverDirectives[];
		static const size_t				 m_locationDirectivesCount;
		static const size_t				 m_serverDirectivesCount;

		void			 saveToken(Vec<std::string> &tokens, std::string &current, const bool inQuotes);
		Vec<std::string> extractTokens(const std::string &line);
		std::string		 joinTokens(const Vec<std::string> &tokens, const size_t start);

		void set(const std::string &key, const Vec<std::string> &tokens, AConfigNode &node);
		void resolveListen(ServerNode &server);

		bool isDuplicate(const ServerNode &server, const Vec<ServerNode> &servers, const size_t start = 0);
		void removeDuplicate(Vec<ServerNode> &servers);

		template <typename T, typename DirectiveType>
		bool applyStringDirective(T &obj, const DirectiveType &directive, const std::string &value);
		template <typename T, typename DirectiveType>
		bool applyBoolDirective(T &obj, const DirectiveType &directive, const std::string &value);
		template <typename T, typename DirectiveType>
		bool applyArrayDirective(T &obj, const DirectiveType &directive, const Vec<std::string> &values);
		template <typename T, typename DirectiveType>
		bool applyNumberDirective(T &obj, const DirectiveType &directive, const std::string &value);
		template <typename T, typename DirectiveType>
		bool applyDirective(
			T					   &object,
			const std::string	   &key,
			const Vec<std::string> &values,
			const DirectiveType		directives[],
			size_t					count);

		VServersNodes groupServersNodes() const;

		void addLocation(Config &config, const Map<LocationNode>::const_iterator &locIt);
		void addConfig(ServerConfigs &config, const ServerNode &serverNode);
		void fillIpAnyMaps(
			Map<unsigned short, SocketAddress> &ipv4AnyPorts,
			Map<unsigned short, SocketAddress> &ipv6AnyPorts) const;

		void createServersConfigs();
		void createHttpConfig();

	public:
		Parser(const std::string &fileContent);
		~Parser();

		const Config		 &getHttpConfig() const;
		const VServerConfigs &getServersConfigs() const;

		void retreive();
		void resolve();
		void removeDuplicate();
		void createConfigs();
};

#include "Parser.tpp"

#endif
