#include "JsonString.hpp"
#include "Logger.hpp"

JsonString::JsonString(const std::string &value) :
	AJson(AJson::STRING),
	m_value(value)
{
}

/**
 *
 * @brief Get the Value object
 *
 * @return A const reference to the string value of this JsonString
 *
 */
const std::string &JsonString::get() const
{
	return m_value;
}



std::ostream &operator<<(std::ostream &os, const JsonString &str)
{
	os << Logger::get(str.get());
	return os;
}