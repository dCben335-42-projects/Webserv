#ifndef CONFIG_CGI_PARAMS_HPP
#define CONFIG_CGI_PARAMS_HPP

#include "Set.hpp"
#include "Map.hpp"
#include "HttpMethod.hpp"

class CgiParams
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

std::ostream &operator<<(std::ostream &os, const CgiParams &cgiParams);

#endif
