#ifndef CONFIG_NODE_HPP
#define CONFIG_NODE_HPP

#include <string>
#include "Map.hpp"
#include "Vec.hpp"

class AConfigNode
{
	protected:
		// clang-format off
		typedef Map<Vec<std::string> > Directives;
		// clang-format on

		Directives		 directives;
		Map<std::string> add_headers;
		Map<std::string> cgi_pass;
		Map<std::string> error_pages;

	public:
		AConfigNode();
		virtual ~AConfigNode();

		// ===== GETTERS =====
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

#endif