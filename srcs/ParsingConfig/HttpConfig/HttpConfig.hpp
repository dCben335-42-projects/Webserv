#ifndef CONFIG_HTTP_HPP
#define CONFIG_HTTP_HPP

#include <string>
#include "Map.hpp"
#include "Vec.hpp"
#include <iostream>
#include "ServerConfig.hpp"


class HttpConfig
{
	private:
		// clang-format off
		typedef Map<Vec<std::string> > Directives;
		// clang-format on

		Directives		  directives;
		Vec<ServerConfig> servers;
		Map<std::string>  add_headers;
		Map<std::string>  cgi_pass;
		Map<std::string>  error_pages;

	public:
		// ===== CONSTRUCTOR =====
		HttpConfig();
		~HttpConfig();

		// ===== GETTERS =====
		const Directives		&getDirectives() const;
		const Vec<ServerConfig> &getServers() const;
		Vec<ServerConfig>		&getServers();
		const Map<std::string>	&getAddHeaders() const;
		const Map<std::string>	&getCgiPass() const;
		const Map<std::string>	&getErrorPages() const;

		// ===== SETTERS =====
		void setDirectiveAt(const std::string &key, const Vec<std::string> &value);
		void setCgiPassAt(const std::string &key, const std::string &value);
		void setErrorPagesAt(const std::string &key, const std::string &value);
		void setAddHeadersAt(const std::string &key, const std::string &value);
};

std::ostream &operator<<(std::ostream &os, const HttpConfig &data);

#endif