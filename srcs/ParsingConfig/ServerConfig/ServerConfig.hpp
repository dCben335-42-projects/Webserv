#ifndef CONFIG_SERVER_HPP
#define CONFIG_SERVER_HPP

#include <string>
#include <iostream>
#include "Map.hpp"
#include "Vec.hpp"
#include "LocationConfig.hpp"

struct ServerConfig
{
	private:
		// clang-format off
		typedef Map<Vec<std::string> > Directives;
		// clang-format on

		Directives			directives;
		Map<LocationConfig> locations;
		Map<std::string>	add_headers;
		Map<std::string>	cgi_pass;
		Map<std::string>	error_pages;

	public:
		// ===== CONSTRUCTOR =====
		ServerConfig();
		~ServerConfig();

		// ===== GETTERS =====
		const Directives		  &getDirectives() const;
		const Map<LocationConfig> &getLocations() const;
		Map<LocationConfig>		  &getLocations();
		const Map<std::string>	  &getAddHeaders() const;
		const Map<std::string>	  &getCgiPass() const;
		const Map<std::string>	  &getErrorPages() const;

		// ===== SETTERS =====
		void setDirectiveAt(const std::string &key, const Vec<std::string> &value);
		void setCgiPassAt(const std::string &key, const std::string &value);
		void setErrorPagesAt(const std::string &key, const std::string &value);
		void setAddHeadersAt(const std::string &key, const std::string &value);
};

std::ostream &operator<<(std::ostream &os, const ServerConfig &data);

#endif