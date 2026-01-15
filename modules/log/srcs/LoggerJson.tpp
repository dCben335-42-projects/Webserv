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
std::string LoggerJson::mapGet(const MapType &m)
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

		if (!is_string<typename MapType::mapped_type>::value)
		{
			Logger::incrementIndent();
			result += "\n";
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
std::string LoggerJson::vectorGet(const VectorType &vec)
{
	(void) vec; // To avoid unused parameter warning for now
	std::string result = "\n" + Logger::indent() + "[\n";
	Logger::incrementIndent();
	for (size_t i = 0; i < vec.size(); ++i)
	{
		result += Logger::indent();
		result += get(vec[i]);
		if (i + 1 < vec.size())
			result += ",\n";
	}
	Logger::decrementIndent();
	result += "\n" + Logger::indent() + "]";
	return result;
}

template <typename SetType>
std::string LoggerJson::setGet(const SetType &set)
{
	std::string result = "\n" + Logger::indent() + "[\n";
	Logger::incrementIndent();
	typename SetType::const_iterator it	 = set.begin();
	typename SetType::const_iterator end = set.end();
	while (it != end)
	{
		result += Logger::indent();
		result += get(*it);
		if (++it != end)
			result += ",\n";
	}
	Logger::decrementIndent();
	result += "\n" + Logger::indent() + "]";
	return result;
}


template <typename SetType>
std::string LoggerJson::setGetAddress(const SetType &set)
{
	std::string result = "\n" + Logger::indent() + "[\n";
	Logger::incrementIndent();
	typename SetType::const_iterator it	 = set.begin();
	typename SetType::const_iterator end = set.end();
	while (it != end)
	{
		result += Logger::indent();
		std::ostringstream oss;
		oss << *it;
		result += oss.str();
		if (++it != end)
			result += ",\n";
	}
	Logger::decrementIndent();
	result += "\n" + Logger::indent() + "]";
	return result;
}




template <typename V>
std::string LoggerJson::get(const V &v)
{
	std::ostringstream oss;
	oss << v;
	return oss.str();
}

template <typename V>
std::string LoggerJson::get(V *ptr)
{
	if (ptr)
		return get(*ptr);
	return "null";
}





template <typename K, typename V>
std::string LoggerJson::get(const std::map<K, V> &m)
{
	return mapGet(m);
}
template <typename K, typename V>
std::string LoggerJson::get(const Map<K, V> &m)
{
	return mapGet(m);
}


template <typename V>
std::string LoggerJson::get(const std::vector<V> &vec)
{
	return vectorGet(vec);
}
template <typename V>
std::string LoggerJson::get(const Vec<V> &vec)
{
	return vectorGet(vec);
}



template <typename V>
std::string LoggerJson::get(const std::set<V> &set)
{
	return setGet(set);
}
template <typename V>
std::string LoggerJson::get(const Set<V> &set)
{
	return setGet(set);
}
template <typename V>
std::string LoggerJson::get(const V &v, const bool getAddress)
{
	return getAddress ? setGetAddress(v) : get(v);
}


#endif