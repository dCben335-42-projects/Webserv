#include "CgiParams.hpp"

Config::CgiParams::CgiParams() :
	m_allowCgi(false),
	m_allowedMethods()
{
}

Config::CgiParams::~CgiParams()
{
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
 * @brief Gets the set of allowed HTTP methods for CGI.
 *
 * @return A set of allowed HTTP methods.
 *
 */
Set<HttpMethod> Config::CgiParams::getAllowedMethods() const
{
	return m_allowedMethods;
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


/**
 *
 * @brief Adds an allowed HTTP method for CGI.
 *
 * @param method The HTTP method to be added.
 *
 */
void Config::CgiParams::addAllowedMethod(HttpMethod method)
{
	m_allowedMethods.insert(method);
}


/**
 *
 * @brief Checks if the given HTTP method is allowed for CGI.
 *
 * @param method The HTTP method to be checked.
 *
 * @return true if the method is allowed, false otherwise.
 *
 */
bool Config::CgiParams::isAllowedMethod(HttpMethod method) const
{
	return m_allowedMethods.contains(method);
}