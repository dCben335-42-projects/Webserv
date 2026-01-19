#ifndef CONFIG_CGI_PARAMS_HPP
#define CONFIG_CGI_PARAMS_HPP

#include <string>
#include "Set.hpp"
#include "Map.hpp"
#include "Config.hpp"
#include "HttpMethod.hpp"

class Config::CgiParams
{
	private:
		bool			m_allowCgi;
		Set<HttpMethod> m_allowedMethods;

	public:
		CgiParams();
		~CgiParams();

		bool			getAllowCgi() const;
		Set<HttpMethod> getAllowedMethods() const;
		void			setAllowCgi(const bool &allowCgi);
		void			addAllowedMethod(HttpMethod method);

		bool isAllowedMethod(HttpMethod method) const;
};


#endif