#ifndef LOGGER_JSON_TPP
#define LOGGER_JSON_TPP

#include "../includes/LoggerJson.hpp"
#include "../includes/Logger.hpp"
#include "Map.hpp"
#include "Vec.hpp"
#include "Set.hpp"
#include "IsType.hpp"

#define RED "\033[31m"
#define RESET "\033[0m"


/**
 *
 * @brief Generates a JSON representation of a map.
 *
 * @param m The map to be converted to JSON.
 * @param getAddress If true, returns the memory address of the values instead of their content.
 *
 * @return A string containing the JSON representation of the map.
 *
 */
template <typename MapType>
std::string LoggerJson::mapGet(const MapType &m, const bool getAddress)
{
	std::string result = "{\n";
	Logger::incrementIndent();

	typename MapType::const_iterator	 it	 = m.begin();
	typename MapType::const_iterator	 end = m.end();
	typedef typename MapType::value_type ValueType;

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
			if (!is_string<ValueType>::value)
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
	result += "}";
	return result;
}


/**
 *
 * @brief Generates a JSON representation of a vector.
 *
 * @param vec The vector to be converted to JSON.
 * @param getAddress If true, returns the memory address of the elements instead of their content.
 *
 * @return A string containing the JSON representation of the vector.
 *
 */
template <typename VectorType>
std::string LoggerJson::vectorGet(const VectorType &vec, const bool getAddress)
{
	std::string result = "[\n";
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


/**
 *
 * @brief Generates a JSON representation of a set.
 *
 * @param set The set to be converted to JSON.
 * @param getAddress If true, returns the memory address of the elements instead of their content.
 *
 * @return A string containing the JSON representation of the set.
 *
 */
template <typename SetType>
std::string LoggerJson::setGet(const SetType &set, const bool getAddress)
{
	std::string result = "[\n";
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


/**
 *
 * @brief Generates a string representation of a value.
 *
 * @param v The value to be converted to a string.
 * @param getAddress If true, returns the memory address of the value instead of its content.
 *
 * @return A string containing the representation of the value.
 *
 */
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

/**
 *
 * @brief Generates a string representation of a pointer.
 *
 * @param ptr The pointer to be converted to a string.
 * @param getAddress If true, returns the memory address of the value instead of its content.
 *
 * @return A string containing the representation of the pointer.
 *
 */
template <typename V>
std::string LoggerJson::get(V *ptr, const bool getAddress)
{
	if (ptr)
		return get(*ptr, getAddress);
	return "null";
}


/**
 *
 * @brief Generates a string representation of a const pointer.
 *
 * @param ptr The const pointer to be converted to a string.
 * @param getAddress If true, returns the memory address of the value instead of its content.
 *
 * @return A string containing the representation of the const pointer.
 *
 */
template <typename V>
std::string LoggerJson::get(V const *ptr, const bool getAddress)
{
	if (ptr)
		return get(*ptr, getAddress);
	return "null";
}


/**
 *
 * @brief Generates a JSON representation of a map.
 *
 * @param m The map to be converted to JSON.
 * @param getAddress If true, returns the memory address of the values instead of their content.
 *
 * @return A string containing the JSON representation of the map.
 *
 */
template <typename K, typename V>
std::string LoggerJson::get(const std::map<K, V> &m, const bool getAddress)
{
	return mapGet(m, getAddress);
}

/**
 *
 * @brief Generates a JSON representation of a Map.
 *
 * @param m The Map to be converted to JSON.
 * @param getAddress If true, returns the memory address of the values instead of their content.
 *
 * @return A string containing the JSON representation of the Map.
 *
 */
template <typename K, typename V>
std::string LoggerJson::get(const Map<K, V> &m, const bool getAddress)
{
	return mapGet(m, getAddress);
}

/**
 *
 * @brief Generates a JSON representation of a vector.
 *
 * @param vec The vector to be converted to JSON.
 * @param getAddress If true, returns the memory address of the elements instead of their content.
 *
 * @return A string containing the JSON representation of the vector.
 *
 */
template <typename V>
std::string LoggerJson::get(const std::vector<V> &vec, const bool getAddress)
{
	return vectorGet(vec, getAddress);
}
/**
 *
 * @brief Generates a JSON representation of a Vec.
 *
 * @param vec The Vec to be converted to JSON.
 * @param getAddress If true, returns the memory address of the elements instead of their content.
 *
 * @return A string containing the JSON representation of the Vec.
 *
 */
template <typename V>
std::string LoggerJson::get(const Vec<V> &vec, const bool getAddress)
{
	return vectorGet(vec, getAddress);
}

/**
 *
 * @brief Generates a JSON representation of a set.
 *
 * @param set The set to be converted to JSON.
 * @param getAddress If true, returns the memory address of the elements instead of their content.
 *
 * @return A string containing the JSON representation of the set.
 *
 */
template <typename V>
std::string LoggerJson::get(const std::set<V> &set, const bool getAddress)
{
	return setGet(set, getAddress);
}
/**
 *
 * @brief Generates a JSON representation of a Set.
 *
 * @param set The Set to be converted to JSON.
 * @param getAddress If true, returns the memory address of the elements instead of their content.
 *
 * @return A string containing the JSON representation of the Set.
 *
 */
template <typename V>
std::string LoggerJson::get(const Set<V> &set, const bool getAddress)
{
	return setGet(set, getAddress);
}


#endif