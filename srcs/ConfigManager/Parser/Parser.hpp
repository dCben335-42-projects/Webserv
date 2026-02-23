#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include "HttpConfig.hpp"




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
	{"host", DirectiveKeyWord::STRING},
	{"port", DirectiveKeyWord::NUMBER},
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

	public:
		Parser(const std::string &fileContent);
		~Parser();

		const HttpConfig &getHttpConfig() const;

		void retreive();
		void parse();
};

#endif
