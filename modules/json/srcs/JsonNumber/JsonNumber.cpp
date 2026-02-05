
#include "JsonNumber.hpp"
#include "Logger.hpp"

JsonNumber::JsonNumber(const double value) :
	AJson(AJson::NUMBER),
	m_value(value)
{
}

/**
 *
 * @brief Gets the value of the JSON number.
 *
 * @return The value of the JSON number.
 *
 */
double JsonNumber::get() const
{
	return m_value;
}

std::ostream &operator<<(std::ostream &os, const JsonNumber &num)
{
	os << Logger::get(num.get());
	return os;
}