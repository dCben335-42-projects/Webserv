#include "CgiParams.hpp"

Config::CgiParams::CgiParams() :
	m_allowCgi(false)
{
}

Config::CgiParams::CgiParams(Map<std::string> binMap) :
	m_allowCgi(true),
	m_binMap(binMap)
{
}


Config::CgiParams::~CgiParams()
{
}

const Map<std::string> &Config::CgiParams::getBinMap() const
{
	return (m_binMap);
}


/**
 *
 * @brief Gets whether CGI is allowed.
 *
 * @return true if CGI is allowed, false otherwise.
 *
 */
bool Config::CgiParams::getAllowCgi() const
{
	return m_allowCgi;
}


/**
 *
 * @brief Sets whether CGI is allowed.
 *
 * @param allowCgi A boolean indicating whether to allow CGI.
 *
 */
void Config::CgiParams::setAllowCgi(const bool &allowCgi)
{
	m_allowCgi = allowCgi;
}
