#include "JsonBool.hpp"
#include "Logger.hpp"

JsonBool::JsonBool(const bool value) :
	AJson(AJson::BOOL),
	m_value(value)
{
}

bool JsonBool::getValue() const
{
	return m_value;
}

std::ostream &operator<<(std::ostream &os, const JsonBool &b)
{
	os << Logger::get(b.getValue());
	return os;
}