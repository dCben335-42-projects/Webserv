#include "CgiParams.hpp"

CgiParams::CgiParams() :
	m_allowCgi(false)
{
}

CgiParams::CgiParams(Map<std::string> binMap) :
	m_allowCgi(true),
	m_binMap(binMap)
{
}


CgiParams::~CgiParams()
{
}

const Map<std::string> &CgiParams::getBinMap() const
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
bool CgiParams::getAllowCgi() const
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
void CgiParams::setAllowCgi(const bool &allowCgi)
{
	m_allowCgi = allowCgi;
}



#include "Logger.hpp"

std::ostream &operator<<(std::ostream &os, const CgiParams &cgiParams)
{
	os << "CgiParams(allowCgi: " << (cgiParams.getAllowCgi() ? "true" : "false") << ", binMap: " << Logger::get(cgiParams.getBinMap()) << ")";
	return os;
}
