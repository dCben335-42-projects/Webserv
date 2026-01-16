#include "../includes/Logger.hpp"

size_t		   Logger::m_indent = 0;
Logger::Target Logger::m_target = Logger::Stdout;
std::ofstream  Logger::m_file;


/**
 *
 * @brief Sets the logging target.
 *
 * @param t The target to set (Stdout, Stderr, or File).
 *
 */
void Logger::setTarget(Target t)
{
	m_target = t;
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
void Logger::setFile(const std::string &path, const FileMode mode)
{
	if (m_target == File)
		closeFile();
	if (mode == Truncate)
		m_file.open(path.c_str(), std::ofstream::out | std::ofstream::trunc);
	else if (mode == Append)
		m_file.open(path.c_str(), std::ofstream::out | std::ofstream::app);
	m_target = File;
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
void Logger::closeFile()
{
	if (m_file.is_open())
		m_file.close();
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
 * @brief Logs a string message with the current indentation.
 *
 * @param str The string message to log.
 *
 */
void Logger::log(const std::string &str)
{
	stream() << indent() << str << std::endl;
}

/**
 *
 * @brief Logs a C-style string message with the current indentation.
 *
 * @param cstr The C-style string message to log.
 *
 */
void Logger::log(const char *cstr)
{
	stream() << indent() << cstr << std::endl;
}

/**
 *
 * @brief Logs a single character with the current indentation.
 *
 * @param c The character to log.
 *
 */
void Logger::log(char c)
{
	stream() << indent() << c << std::endl;
}


/**
 *
 * @brief Increments the current indentation level.
 *
 * @return The new indentation level after incrementing.
 *
 */
size_t Logger::incrementIndent()
{
	return ++m_indent;
}


/**
 *
 * @brief Decrements the current indentation level.
 *
 * @return The new indentation level after decrementing.
 *
 */
size_t Logger::decrementIndent()
{
	if (m_indent > 0)
		--m_indent;
	return m_indent;
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
