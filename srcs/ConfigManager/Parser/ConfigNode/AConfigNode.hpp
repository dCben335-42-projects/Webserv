#ifndef CONFIG_NODE_HPP
#define CONFIG_NODE_HPP

#include <string>
#include "Map.hpp"
#include "Vec.hpp"

class AConfigNode
{
	public:
		// clang-format off
		typedef Map<Vec<std::string> > Directives;
		typedef Directives::const_iterator DirectivesConstIt;
		// clang-format on

	protected:
		Directives m_directives;

		Map<std::string> m_addHeaders;
		Map<std::string> m_cgiPass;
		Map<std::string> m_errorPages;

	public:
		AConfigNode();
		virtual ~AConfigNode();

		const Directives	   &getDirectives() const;
		const Map<std::string> &getAddHeaders() const;
		const Map<std::string> &getCgiPass() const;
		const Map<std::string> &getErrorPages() const;

		bool					hasDirective(const std::string &key) const;
		const Vec<std::string> &getDirective(const std::string &key) const;

		void setDirective(const std::string &key, const Vec<std::string> &value);
		void setCgiPass(const std::string &key, const std::string &value);
		void setErrorPages(const std::string &key, const std::string &value);
		void setAddHeaders(const std::string &key, const std::string &value);

		void handleSet(const std::string &key, const std::string &value);
};

#endif