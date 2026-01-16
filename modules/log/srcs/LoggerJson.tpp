#ifndef LOGGER_JSON_TPP
#define LOGGER_JSON_TPP

#include "../includes/LoggerJson.hpp"
#include "../includes/Logger.hpp"
#include "Map.hpp"
#include "Vector.hpp"
#include "Set.hpp"
#include "IsType.hpp"

#define RED "\033[31m"
#define RESET "\033[0m"

template <typename MapType>
std::string LoggerJson::mapGet(const MapType &m, const bool getAddress)
{
	std::string result = "\n" + Logger::indent() + "{\n";
	Logger::incrementIndent();

	typename MapType::const_iterator it	 = m.begin();
	typename MapType::const_iterator end = m.end();


	while (it != end)
	{
		result += Logger::indent();
		result += "\"";
		result += get(it->first);
		result += "\"";
		result += ": ";

		if (getAddress)
		{
			std::ostringstream oss;
			oss << &it->second;
			result += oss.str();
		}
		else
		{
			if (!is_string<typename MapType::mapped_type>::value)
			{
				result += "\n";
				Logger::incrementIndent();
				result += Logger::indent();
				result += get(it->second);
				Logger::decrementIndent();
			}
			else
			{
				result += "\"";
				result += get(it->second);
				result += "\"";
			}
		}

		if (++it != end)
			result += ",\n";
		else
			result += "\n";
	}
	Logger::decrementIndent();
	result += Logger::indent() + "}";
	return result;
}


template <typename VectorType>
std::string LoggerJson::vectorGet(const VectorType &vec, const bool getAddress)
{
	(void) vec; // To avoid unused parameter warning for now
	std::string result = "\n" + Logger::indent() + "[\n";
	Logger::incrementIndent();
	for (size_t i = 0; i < vec.size(); ++i)
	{
		result += Logger::indent();
		result += get(vec[i], getAddress);
		if (i + 1 < vec.size())
			result += ",\n";
	}
	Logger::decrementIndent();
	result += "\n" + Logger::indent() + "]";
	return result;
}

template <typename SetType>
std::string LoggerJson::setGet(const SetType &set, const bool getAddress)
{
	std::string result = "\n" + Logger::indent() + "[\n";
	Logger::incrementIndent();
	typename SetType::const_iterator it	 = set.begin();
	typename SetType::const_iterator end = set.end();
	while (it != end)
	{
		result += Logger::indent();
		result += get(*it, getAddress);
		if (++it != end)
			result += ",\n";
	}
	Logger::decrementIndent();
	result += "\n" + Logger::indent() + "]";
	return result;
}


template <typename V>
std::string LoggerJson::get(const V &v, const bool getAddress)
{
	std::ostringstream oss;
	if (getAddress)
	{
		oss << &v;
		return oss.str();
	}
	oss << v;
	return oss.str();
}

template <typename V>
std::string LoggerJson::get(V *ptr, const bool getAddress)
{
	if (ptr)
		return get(*ptr, getAddress);
	return "null";
}





template <typename K, typename V>
std::string LoggerJson::get(const std::map<K, V> &m, const bool getAddress)
{
	return mapGet(m, getAddress);
}
template <typename K, typename V>
std::string LoggerJson::get(const Map<K, V> &m, const bool getAddress)
{
	return mapGet(m, getAddress);
}


template <typename V>
std::string LoggerJson::get(const std::vector<V> &vec, const bool getAddress)
{
	return vectorGet(vec, getAddress);
}
template <typename V>
std::string LoggerJson::get(const Vec<V> &vec, const bool getAddress)
{
	return vectorGet(vec, getAddress);
}


template <typename V>
std::string LoggerJson::get(const std::set<V> &set, const bool getAddress)
{
	return setGet(set, getAddress);
}
template <typename V>
std::string LoggerJson::get(const Set<V> &set, const bool getAddress)
{
	return setGet(set, getAddress);
}


#endif