#include "../includes/Logger.hpp"

size_t		   Logger::m_indent = 0;
Logger::Target Logger::m_target = Logger::Stdout;
std::ofstream  Logger::m_file;
Logger::Format Logger::m_format = Logger::Json;

std::string Logger::m_customPrefix = "";
std::string Logger::m_customColor  = "";

const std::string Logger::LOG_INFO_PREFIX  = LOG_DEFAULT_INFO_PREFIX;
const std::string Logger::LOG_WARN_PREFIX  = LOG_DEFAULT_WARN_PREFIX;
const std::string Logger::LOG_ERROR_PREFIX = LOG_DEFAULT_ERROR_PREFIX;
const std::string Logger::LOG_DEBUG_PREFIX = LOG_DEFAULT_DEBUG_PREFIX;

/**
 *
 * @brief Returns the singleton instance of the Logger class.
 *
 * @note It is used internally to ensure a single instance of Logger.
 *
 * @return A reference to the Logger instance.
 *
 */
Logger &Logger::instance()
{
	static Logger instance;
	return instance;
}

/**
 *
 * @brief Returns the appropriate output stream based on the current target.
 *
 * @return A reference to the selected output stream (std::cout, std::cerr, or file stream).
 *
 */
std::ostream &Logger::stream()
{
	switch (m_target)
	{
		case Stderr:
			return std::cerr;
		case File:
			return m_file;
		default:
			return std::cout;
	}
}





/**
 *
 * @brief Sets the custom color code for log messages.
 *
 * @param colorCode The color code to set (e.g., ANSI escape codes).
 *
 */
Logger &Logger::setCustomPrefix(const std::string &prefix)
{
	m_customPrefix = prefix;
	return instance();
}
/**
 *
 * @brief Sets the custom color code for log messages.
 *
 * @param colorCode The color code to set (e.g., ANSI escape codes).
 *
 */
Logger &Logger::setCustomColor(const std::string &colorCode)
{
	m_customColor = colorCode;
	return instance();
}
/**
 *
 * @brief Sets the logging format.
 *
 * @param format The format to set (PlainText or Json).
 *
 */
Logger &Logger::setFormat(const Format format)
{
	m_format = format;
	return instance();
}
/**
 *
 * @brief Sets the logging target.
 *
 * @param t The target to set (Stdout, Stderr, or File).
 *
 */
Logger &Logger::setTarget(Target t)
{
	m_target = t;
	return instance();
}
/**
 *
 * @brief Sets the log file path and mode.
 *
 * @param path The file path to log to.
 * @param mode The file mode (Append or Truncate).
 *
 * @note If a file is already open, it will be closed before opening the new file.
 *
 */
Logger &Logger::setFile(const std::string &path, const FileMode mode)
{
	if (m_target == File)
		closeFile();
	if (mode == Truncate)
		m_file.open(path.c_str(), std::ofstream::out | std::ofstream::trunc);
	else if (mode == Append)
		m_file.open(path.c_str(), std::ofstream::out | std::ofstream::app);
	m_target = File;
	return instance();
}



/**
 *
 * @brief Retrieves the current logging format.
 *
 * @return The current logging format (PlainText or Json).
 *
 */
Logger::Format Logger::getFormat()
{
	return m_format;
}
/**
 *
 * @brief Retrieves the current logging target.
 *
 * @return The current logging target (Stdout, Stderr, or File).
 *
 */
Logger::Target Logger::getTarget()
{
	return m_target;
}
/**
 *
 * @brief Retrieves the custom prefix for log messages.
 *
 * @return The custom prefix string.
 *
 */
std::string Logger::getCustomPrefix()
{
	return m_customPrefix;
}
/**
 *
 * @brief Retrieves the custom color code for log messages.
 *
 * @return The custom color code string.
 *
 */
std::string Logger::getCustomColor()
{
	return m_customColor;
}




/**
 *
 * @brief Closes the log file if it is open.
 *
 * @note This function is called when changing the log target.
 *
 * @note It should also be called before the program exits to ensure all data is flushed.
 *
 */
Logger &Logger::closeFile()
{
	if (m_file.is_open())
		m_file.close();
	return instance();
}




/**
 *
 * @brief Increments the current indentation level.
 *
 * @return The new indentation level after incrementing.
 *
 */
Logger &Logger::incrementIndent()
{
	++m_indent;
	return instance();
}
/**
 *
 * @brief Decrements the current indentation level.
 *
 * @return The new indentation level after decrementing.
 *
 */
Logger &Logger::decrementIndent()
{
	if (m_indent > 0)
		--m_indent;
	return instance();
}
/**
 *
 * @brief Retrieves the current indentation level.
 *
 * @return The current indentation level.
 *
 */
size_t Logger::getIndent()
{
	return m_indent;
}
/**
 *
 * @brief Generates a string representing the current indentation.
 *
 * @return A string consisting of spaces corresponding to the current indentation level.
 *
 */
std::string Logger::indent()
{
	return std::string(m_indent * INDENT_SIZE, INDENT_CHAR);
}





/**
 *
 * @brief Logs a string message with the current indentation.
 *
 * @param str The string message to log.
 *
 */
Logger &Logger::log(const std::string &str)
{
	stream() << indent() << str << std::endl;
	return instance();
}
/**
 *
 * @brief Logs a C-style string message with the current indentation.
 *
 * @param cstr The C-style string message to log.
 *
 */
Logger &Logger::log(const char *cstr)
{
	stream() << indent() << cstr << std::endl;
	return instance();
}
/**
 *
 * @brief Logs a single character with the current indentation.
 *
 * @param c The character to log.
 *
 */
Logger &Logger::log(char c)
{
	stream() << indent() << c << std::endl;
	return instance();
}





/**
 *
 * @brief Logs a generic value with the current indentation.
 *
 * @param v The value to log.
 *
 */
Logger &Logger::info(const std::string &str)
{
	return Logger::log(LOG_INFO_COLOR + LOG_INFO_PREFIX + str + LOG_RESET_COLOR);
}
/**
 *
 * @brief Logs a C-style string message as info with the current indentation.
 *
 * @param cstr The C-style string message to log.
 *
 */
Logger &Logger::info(const char *cstr)
{
	return Logger::log(LOG_INFO_COLOR + LOG_INFO_PREFIX + std::string(cstr) + LOG_RESET_COLOR);
}
/**
 *
 * @brief Logs a single character as info with the current indentation.
 *
 * @param c The character to log.
 *
 */
Logger &Logger::info(char c)
{
	return Logger::log(LOG_INFO_COLOR + LOG_INFO_PREFIX + std::string(1, c) + LOG_RESET_COLOR);
}





/**
 *
 * @brief Logs a generic value as info with the current indentation.
 *
 * @param v The value to log.
 *
 */
Logger &Logger::error(const std::string &str)
{
	return Logger::log(LOG_ERROR_COLOR + LOG_ERROR_PREFIX + str + LOG_RESET_COLOR);
}
/**
 *
 * @brief Logs a C-style string message as an error with the current indentation.
 *
 * @param cstr The C-style string message to log.
 *
 */
Logger &Logger::error(const char *cstr)
{
	return Logger::log(LOG_ERROR_COLOR + LOG_ERROR_PREFIX + std::string(cstr) + LOG_RESET_COLOR);
}
/**
 *
 * @brief Logs a single character as an error with the current indentation.
 *
 * @param c The character to log.
 *
 */
Logger &Logger::error(char c)
{
	return Logger::log(LOG_ERROR_COLOR + LOG_ERROR_PREFIX + std::string(1, c) + LOG_RESET_COLOR);
}





/**
 *
 * @brief Logs a generic value as an error with the current indentation.
 *
 * @param v The value to log.
 *
 */
Logger &Logger::warn(const std::string &str)
{
	return Logger::log(LOG_WARN_COLOR + LOG_WARN_PREFIX + str + LOG_RESET_COLOR);
}
/**
 *
 * @brief Logs a C-style string message as a warning with the current indentation.
 *
 * @param cstr The C-style string message to log.
 *
 */
Logger &Logger::warn(const char *cstr)
{
	return Logger::log(LOG_WARN_COLOR + LOG_WARN_PREFIX + std::string(cstr) + LOG_RESET_COLOR);
}
/**
 *
 * @brief Logs a single character as a warning with the current indentation.
 *
 * @param c The character to log.
 *
 */
Logger &Logger::warn(char c)
{
	return Logger::log(LOG_WARN_COLOR + LOG_WARN_PREFIX + std::string(1, c) + LOG_RESET_COLOR);
}





/**
 *
 * @brief Logs a generic value as a debug message with the current indentation.
 *
 * @param v The value to log.
 *
 */
Logger &Logger::debug(const std::string &str)
{
	return Logger::log(LOG_DEBUG_COLOR + LOG_DEBUG_PREFIX + str + LOG_RESET_COLOR);
}
/**
 *
 * @brief Logs a C-style string message as a debug message with the current indentation.
 *
 * @param cstr The C-style string message to log.
 *
 */
Logger &Logger::debug(const char *cstr)
{
	return Logger::log(LOG_DEBUG_COLOR + LOG_DEBUG_PREFIX + std::string(cstr) + LOG_RESET_COLOR);
}
/**
 *
 * @brief Logs a single character as a debug message with the current indentation.
 *
 * @param c The character to log.
 *
 */
Logger &Logger::debug(char c)
{
	return Logger::log(LOG_DEBUG_COLOR + LOG_DEBUG_PREFIX + std::string(1, c) + LOG_RESET_COLOR);
}



/**
 *
 * @brief Logs a generic value as a debug message with the current indentation.
 *
 * @param v The value to log.
 *
 */
Logger &Logger::custom(const std::string &str)
{
	return Logger::log(m_customColor + m_customPrefix + str + LOG_RESET_COLOR);
}

/**
 *
 * @brief Logs a C-style string message as a custom message with the current indentation.
 *
 * @param cstr The C-style string message to log.
 *
 */
Logger &Logger::custom(const char *cstr)
{
	return Logger::log(m_customColor + m_customPrefix + std::string(cstr) + LOG_RESET_COLOR);
}
/**
 *
 * @brief Logs a single character as a custom message with the current indentation.
 * @param c The character to log.
 */
Logger &Logger::custom(char c)
{
	return Logger::log(m_customColor + m_customPrefix + std::string(1, c) + LOG_RESET_COLOR);
}