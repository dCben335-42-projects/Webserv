#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include "HttpConfig.hpp"



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

struct DirectiveKeyWord
{
		enum Type
		{
			ARRAY,
			NUMBER,
			STRING,
			MAP,
			BOOL,
		};
		const char *key;
		const Type	returnType;
};


const DirectiveKeyWord configKeyWords[] = {
	{"listen", DirectiveKeyWord::STRING},
	{"server_name", DirectiveKeyWord::STRING},
	{"root", DirectiveKeyWord::STRING},
	{"alias", DirectiveKeyWord::STRING},
	{"client_max_body_size", DirectiveKeyWord::NUMBER},
	{"autoindex", DirectiveKeyWord::BOOL},
	{"methods", DirectiveKeyWord::ARRAY},
	{"index", DirectiveKeyWord::ARRAY},
	{"error_page", DirectiveKeyWord::MAP},
	{"add_header", DirectiveKeyWord::MAP},
	{"cgi_pass", DirectiveKeyWord::MAP},
	{NULL, DirectiveKeyWord::STRING}};



class Parser
{
	private:
		HttpConfig		   m_http;
		const std::string &m_content;

		Vec<std::string> extractTokens(const std::string &line);
		std::string		 joinTokens(const Vec<std::string> &tokens, const size_t start);

		void set(const std::string &key, const Vec<std::string> &tokens, AConfigNode &node);
		void resolveListen(ServerConfig &server);

		bool isDuplicate(const ServerConfig &server, const Vec<ServerConfig> &servers, const size_t start = 0);
		void removeDuplicateSocketAddresses(Vec<ServerConfig> &servers);


	public:
		Parser(const std::string &fileContent);
		~Parser();

		const HttpConfig &getHttpConfig() const;

		void retreive();
		void parse();
};

#endif
