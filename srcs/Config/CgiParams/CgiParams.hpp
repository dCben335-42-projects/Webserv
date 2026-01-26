#ifndef CONFIG_CGI_PARAMS_HPP
#define CONFIG_CGI_PARAMS_HPP

#include "Set.hpp"
#include "Map.hpp"
#include "HttpMethod.hpp"
#include "Config.hpp"

class Config::CgiParams
{
	private:
		bool			 m_allowCgi;
		Map<std::string> m_binMap;

	public:
		CgiParams();
		CgiParams(Map<std::string> binMap);
		~CgiParams();

		bool					getAllowCgi() const;
		const Map<std::string> &getBinMap() const;

		void setAllowCgi(const bool &allowCgi);
};

#endif
