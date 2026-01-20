#ifndef LOGGER_TPP
#define LOGGER_TPP

#include "Logger.hpp"
#include "LoggerJson.hpp"


/**
 *
 * @brief Logs a value of any type with the current indentation.
 *
 * @param v The value to log.
 *
 */
template <typename V>
void Logger::log(const V &v)
{
	stream() << indent() << LoggerJson::get(v) << std::endl;
}


/**
 *
 * @brief Logs a pointer to a value with the current indentation.
 *
 * @param ptr The pointer to the value to log.
 *
 */
template <typename V>
void Logger::log(V *ptr)
{
	if (ptr)
	{
		log(*ptr);
	}
	else
		stream() << indent() << "null" << std::endl;
}

/**
 *
 * @brief Logs a const pointer to a value with the current indentation.
 *
 * @param ptr The const pointer to the value to log.
 *
 */
template <typename V>
void Logger::log(V const *ptr)
{
	if (ptr)
		log(*ptr);
	else
		stream() << indent() << "null" << std::endl;
}

/**
 *
 * @brief Logs a vector with the current indentation.
 *
 * @param vec The vector to log.
 *
 */
template <typename V>
void Logger::log(const std::vector<V> &vec)
{
	stream() << indent() << LoggerJson::get(vec) << std::endl;
}



/**
 *
 * @brief Logs a Vec with the current indentation.
 *
 * @param vec The Vec to log.
 *
 */
template <typename V>
void Logger::log(const Vec<V> &vec)
{
	stream() << indent() << LoggerJson::get(vec) << std::endl;
}



/**
 *
 * @brief Logs a map with the current indentation.
 *
 * @param m The map to log.
 *
 */
template <typename K, typename V>
void Logger::log(const std::map<K, V> &m)
{
	stream() << indent() << LoggerJson::get(m) << std::endl;
}


/**
 *
 * @brief Logs a Map with the current indentation.
 *
 * @param m The Map to log.
 *
 */
template <typename K, typename V>
void Logger::log(const Map<K, V> &m)
{
	stream() << indent() << LoggerJson::get(m) << std::endl;
}

/**
 *
 * @brief Logs a set with the current indentation.
 *
 * @param set The set to log.
 *
 */
template <typename V>
void Logger::log(const std::set<V> &set)
{
	stream() << indent() << LoggerJson::get(set) << std::endl;
}


/**
 *
 * @brief Logs a Set with the current indentation.
 *
 * @param set The Set to log.
 *
 */
template <typename V>
void Logger::log(const Set<V> &set)
{
	stream() << indent() << LoggerJson::get(set) << std::endl;
}


#endif