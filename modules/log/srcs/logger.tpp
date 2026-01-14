#ifndef UTILS_TPP
#define UTILS_TPP

#include "./Log/Log.hpp"
#include "String.hpp"
#include <sstream>
#include <set>

template <typename V>
void log(const V &v)
{
	std::cout << v;
}
template <typename V>
void log(V *ptr)
{
	if (ptr)
		log(*ptr);
	else
		std::cout << Log::indent() << "null";
}

template <typename V>
std::string getLogJson(const V &v)
{
	std::ostringstream oss;

	oss << v;
	return oss.str();
}

template <typename V>
std::string getLogJson(const V *ptr)
{
	if (ptr)
		return getLogJson(*ptr);
	return "null";
}

#endif
