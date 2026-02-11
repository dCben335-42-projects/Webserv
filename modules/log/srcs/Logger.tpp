#ifndef LOG_LOGGER_TPP
#define LOG_LOGGER_TPP

#include "Logger.hpp"
#include "LoggerJson.hpp"
#include "LoggerPlain.hpp"


/**
 *
 * @brief Get the string representation of a value of any type.
 *
 * @param v The value to get the string representation of.
 *
 * @return std::string The string representation of the value.
 *
 */
template <typename T>
std::string Logger::get(const T &v)
{
	if (m_format == PlainText)
		return LoggerPlain::get(v);
	return LoggerJson::get(v);
}
/**
 *
 * @brief Get the string representation of a pointer to a value.
 *
 * @param ptr The pointer to the value to get the string representation of.
 *
 * @return std::string The string representation of the value pointed to.
 *
 */
template <typename T>
std::string Logger::get(T *ptr)
{
	if (!ptr)
	{
		return "null";
	}
	if (m_format == PlainText)
		return LoggerPlain::get(*ptr);
	return LoggerJson::get(*ptr);
}


/**
 *
 * @brief Logs a value of any type with the current indentation.
 *
 * @param v The value to log.
 *
 */
template <typename V>
Logger &Logger::log(const V &v)
{
	stream() << indent() << Logger::get(v) << std::endl;
	return m_instance;
}


/**
 *
 * @brief Logs a pointer to a value with the current indentation.
 *
 * @param ptr The pointer to the value to log.
 *
 */
template <typename V>
Logger &Logger::log(V *ptr)
{
	if (ptr)
	{
		log(*ptr);
	}
	else
		stream() << indent() << "null" << std::endl;
	return m_instance;
}



/**
 *
 * @brief Logs an info message with the current indentation.
 *
 * @param v The value to log as an info message.
 *
 */
template <typename V>
Logger &Logger::info(const V &v)
{
	return Logger::log(LOG_INFO_PREFIX + Logger::get(v) + LOG_RESET_COLOR);
}
/**
 *
 * @brief Logs a pointer to a value as an info message with the current indentation.
 *
 * @param ptr The pointer to the value to log as an info message.
 *
 */
template <typename V>
Logger &Logger::info(V *ptr)
{
	if (ptr)
	{
		Logger::info(*ptr);
	}
	else
		stream() << indent() << "null" << std::endl;
	return m_instance;
}




/**
 *
 * @brief Logs an error message with the current indentation.
 *
 * @param v The value to log as an error message.
 *
 */
template <typename V>
Logger &Logger::error(const V &v)
{
	return Logger::log(LOG_ERROR_PREFIX + Logger::get(v) + LOG_RESET_COLOR);
}
/**
 *
 * @brief Logs a pointer to a value as an error message with the current indentation.
 *
 * @param ptr The pointer to the value to log as an error message.
 *
 */
template <typename V>
Logger &Logger::error(V *ptr)
{
	if (ptr)
	{
		Logger::error(*ptr);
	}
	else
		stream() << indent() << "null" << std::endl;
	return m_instance;
}



/**
 *
 * @brief Logs a warning message with the current indentation.
 *
 * @param v The value to log as a warning message.
 *
 */
template <typename V>
Logger &Logger::warn(const V &v)
{
	return Logger::log(LOG_WARN_PREFIX + Logger::get(v) + LOG_RESET_COLOR);
}
/**
 *
 * @brief Logs a pointer to a value as a warning message with the current indentation.
 *
 * @param ptr The pointer to the value to log as a warning message.
 *
 */
template <typename V>
Logger &Logger::warn(V *ptr)
{
	if (ptr)
	{
		Logger::warn(*ptr);
	}
	else
		stream() << indent() << "null" << std::endl;
	return m_instance;
}

/**
 *
 * @brief Logs a debug message with the current indentation.
 *
 * @param v The value to log as a debug message.
 *
 */
template <typename V>
Logger &Logger::debug(const V &v)
{
	return Logger::log(LOG_DEBUG_PREFIX + Logger::get(v) + LOG_RESET_COLOR);
}
/**
 *
 * @brief Logs a pointer to a value as a debug message with the current indentation.
 *
 * @param ptr The pointer to the value to log as a debug message.
 *
 */
template <typename V>
Logger &Logger::debug(V *ptr)
{
	if (ptr)
	{
		Logger::debug(*ptr);
	}
	else
		stream() << indent() << "null" << std::endl;
	return m_instance;
}



/**
 *
 * @brief Logs a custom message with the current indentation.
 *
 * @param v The value to log as a custom message.
 *
 */
template <typename V>
Logger &Logger::custom(const V &v)
{
	return Logger::log(m_customColor + m_customPrefix + Logger::get(v) + LOG_RESET_COLOR);
}
/**
 *
 * @brief Logs a pointer to a value as a custom message with the current indentation.
 *
 * @param ptr The pointer to the value to log as a custom message.
 *
 */
template <typename V>
Logger &Logger::custom(V *ptr)
{
	if (ptr)
	{
		Logger::custom(*ptr);
	}
	else
		stream() << indent() << "null" << std::endl;
	return m_instance;
}

#endif
