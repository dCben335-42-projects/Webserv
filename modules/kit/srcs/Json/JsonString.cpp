#include "JsonString.hpp"

JsonString::JsonString() : m_value("")
{
}

JsonString::JsonString(const std::string &val) : m_value(val)
{
}

JsonString::JsonString(const JsonString &other) : m_value(other.m_value)
{
}

JsonString &JsonString::operator=(const JsonString &other)
{
	if (this != &other)
	{
		m_value = other.m_value;
	}
	return *this;
}

const std::string &JsonString::getValue() const
{
	return m_value;
}

void JsonString::setValue(const std::string &val)
{
	m_value = val;
}

std::string JsonString::toString() const
{
	std::string result = "\"";
	for (size_t i = 0; i < m_value.size(); ++i)
	{
		char c = m_value[i];
		switch (c)
		{
			case '\"':
				result += "\\\"";
				break;
			case '\\':
				result += "\\\\";
				break;
			case '\b':
				result += "\\b";
				break;
			case '\f':
				result += "\\f";
				break;
			case '\n':
				result += "\\n";
				break;
			case '\r':
				result += "\\r";
				break;
			case '\t':
				result += "\\t";
				break;
			default:
				result += c;
				break;
		}
	}
	result += "\"";
	return result;
}