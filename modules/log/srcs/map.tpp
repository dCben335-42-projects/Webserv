#ifndef LOG_MAP_TPP
#define LOG_MAP_TPP

#include "./Log/Log.hpp"
#include "Map.hpp"
#include <IsType.hpp>


template <typename MapType>
std::string getMapLogJson(const MapType &m)
{
	std::string result = "{\n";
	Log::incrementIndent();

	typename MapType::const_iterator it	 = m.begin();
	typename MapType::const_iterator end = m.end();

	while (it != end)
	{
		result += Log::indent();
		result += "\"";
		result += it->first;
		result += "\": ";
		if (!is_string<typename MapType::mapped_type>::value)
		{
			Log::incrementIndent();
			result += "\n";
			result += getLogString(it->second);
			Log::decrementIndent();
		}
		else
		{
			result += "\"";
			result += it->second;
			result += "\"";
		}
		if (++it != end)
			result += ",\n";
	}
	Log::decrementIndent();
	result += "\n" + Log::indent() + "}";
	return result;
}

template <typename MapType>
void logMapJson(const MapType &m)
{
	std::cout << Log::indent() << "{\n";
	Log::incrementIndent();

	typename MapType::const_iterator it	 = m.begin();
	typename MapType::const_iterator end = m.end();

	while (it != end)
	{
		std::cout << Log::indent();
		std::cout << "\"";
		log(it->first);
		std::cout << "\"";
		std::cout << ": ";
		if (!is_string<typename MapType::mapped_type>::value)
		{
			Log::incrementIndent();
			std::cout << "\n";
			log(it->second);
			Log::decrementIndent();
		}
		else
		{
			std::cout << "\"";
			log(it->second);
			std::cout << "\"";
		}
		if (++it != end)
			std::cout << ",\n";
	}

	Log::decrementIndent();
	std::cout << "\n"
			  << Log::indent() << "}";
}

template <typename K, typename V>
void log(const std::map<K, V> &m)
{
	logMapJson(m);
}

template <typename K, typename V>
void log(const Map<K, V> &m)
{
	logMapJson(m);
}

template <typename K, typename V>
std::string getLogJson(const Map<K, V> &m)
{
	return getMapLogJson(m);
}

template <typename K, typename V>
std::string getLogJson(const std::map<K, V> &m)
{
	return getMapLogJson(m);
}

#endif