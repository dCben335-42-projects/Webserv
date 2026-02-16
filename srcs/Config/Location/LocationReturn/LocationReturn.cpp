#include "LocationReturn.hpp"


LocationReturn::LocationReturn() :
	m_target(),
	m_statusCode(0)
{
}

LocationReturn::LocationReturn(const int statusCode, const std::string &target)
{
	m_target	 = target;
	m_statusCode = statusCode;
}

LocationReturn::~LocationReturn()
{
}

/**
 *
 * @brief Gets the target of the location return.
 *
 * @return A constant reference to the target string.
 *
 */
const std::string &LocationReturn::getTarget() const
{
	return m_target;
}

/**
 *
 * @brief Gets the status code of the location return.
 *
 * @return The status code as an integer.
 *
 */
unsigned short LocationReturn::getStatusCode() const
{
	return m_statusCode;
}


/**
 *
 * @brief Sets the target of the location return.
 *
 * @param target The target string to be set.
 *
 */
void LocationReturn::setTarget(const std::string &target)
{
	m_target = target;
}

/**
 *
 * @brief Sets the status code of the location return.
 *
 * @param statusCode The status code to be set as an integer.
 *
 */
void LocationReturn::setStatusCode(const int statusCode)
{
	m_statusCode = statusCode;
}