#ifndef CONFIG_LOCATION_HPP
#define CONFIG_LOCATION_HPP

#include <string>
#include "Map.hpp"
#include "Vec.hpp"
#include <iostream>

struct LocationConfig
{
	private:
		// clang-format off
		typedef Map<Vec<std::string> > Directives;
		// clang-format on

		std::string		 path;
		Directives		 directives;
		Map<std::string> add_headers;
		Map<std::string> cgi_pass;
		Map<std::string> error_pages;

	public:
		// ===== CONSTRUCTOR =====
		LocationConfig();
		~LocationConfig();

		// ===== GETTERS =====
		const std::string	   &getPath() const;
		std::string			   &getPath();
		const Directives	   &getDirectives() const;
		const Map<std::string> &getAddHeaders() const;
		const Map<std::string> &getCgiPass() const;
		const Map<std::string> &getErrorPages() const;

		// ===== SETTERS =====
		void setDirectiveAt(const std::string &key, const Vec<std::string> &value);
		void setCgiPassAt(const std::string &key, const std::string &value);
		void setErrorPagesAt(const std::string &key, const std::string &value);
		void setAddHeadersAt(const std::string &key, const std::string &value);
};

std::ostream &operator<<(std::ostream &os, const LocationConfig &data);

#endif