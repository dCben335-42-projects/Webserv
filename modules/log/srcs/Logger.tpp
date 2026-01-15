#ifndef LOGGER_TPP
#define LOGGER_TPP

#include "Logger.hpp"
#include "LoggerJson.hpp"


template <typename V>
void Logger::log(const V &v)
{
	stream() << indent() << LoggerJson::get(v) << std::endl;
}

template <typename V>
void Logger::log(V *ptr)
{
	if (ptr)
		log(*ptr);
	else
		stream() << indent() << "null" << std::endl;
}


template <typename V>
void Logger::log(const std::vector<V> &vec)
{
	stream() << indent() << LoggerJson::get(vec) << std::endl;
}

template <typename V>
void Logger::log(const Vec<V> &vec)
{
	stream() << indent() << LoggerJson::get(vec) << std::endl;
}


template <typename K, typename V>
void Logger::log(const std::map<K, V> &m)
{
	stream() << indent() << LoggerJson::get(m) << std::endl;
}

template <typename K, typename V>
void Logger::log(const Map<K, V> &m)
{
	stream() << indent() << LoggerJson::get(m) << std::endl;
}


template <typename V>
void Logger::log(const std::set<V> &set)
{
	stream() << indent() << LoggerJson::get(set) << std::endl;
}

template <typename V>
void Logger::log(const Set<V> &set)
{
	stream() << indent() << LoggerJson::get(set) << std::endl;
}


#endif