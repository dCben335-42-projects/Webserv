#include "JsonBool.hpp"
#include "Logger.hpp"

JsonBool::JsonBool(const bool value) :
	AJson(AJson::BOOL),
	m_value(value)
{
}

/**
 *
 * @brief Gets the boolean value of the JSON boolean.
 *
 * @return The boolean value of the JSON boolean.
 *
 */
bool JsonBool::get() const
{
	return m_value;
}

/**
 *
 * @brief Gets the string representation of the JSON boolean value.
 *
 * @return A string containing "true" if the value is true, or "false" if the value is false.
 *
 */
std::string JsonBool::getString() const
{
	return m_value ? "true" : "false";
}



std::ostream &operator<<(std::ostream &os, const JsonBool &b)
{
	os << Logger::get(b.getString());
	return os;
}