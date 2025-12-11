#include "./Log.hpp"
#include <iostream>

size_t Log::m_indent = 0;

size_t Log::getIndent()
{
	return m_indent;
}

size_t Log::incrementIndent()
{
	return ++m_indent;
}

size_t Log::decrementIndent()
{
	if (m_indent > 0)
		--m_indent;
	return m_indent;
}

std::string Log::indent()
{
	return std::string(Log::getIndent() * TAP_SIZE, SPACE);
}

void log(const char *s)
{
	if (s)
		std::cout << s;
	else
		std::cout << Log::indent() << "null";
}


std::string getLogJson(const std::string &s)
{
	return s;
}