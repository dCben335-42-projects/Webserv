#ifndef LOG_LOGGER_HPP
#define LOG_LOGGER_HPP

#include <fstream>
#include <string>

#include <Map.hpp>
#include "Vec.hpp"
#include <Set.hpp>

#define LOG_INFO_COLOR "\033[1;34m"
#define LOG_ERROR_COLOR "\033[1;31m"
#define LOG_WARN_COLOR "\033[1;33m"
#define LOG_DEBUG_COLOR "\033[1;32m"
#define LOG_RESET_COLOR "\033[0m"

#define LOG_DEFAULT_INFO_PREFIX "[INFO] "
#define LOG_DEFAULT_ERROR_PREFIX "[ERROR] "
#define LOG_DEFAULT_WARN_PREFIX "[WARN] "
#define LOG_DEFAULT_DEBUG_PREFIX "[DEBUG] "



/**
 *
 * @brief A static logging utility class.
 *
 * @note This class provides static methods for logging messages to various targets
 * (stdout, stderr, or file) with different formats (plain text or JSON). It supports
 * indentation levels for better readability of nested logs.
 *
 * @usage Logger::info("This is an info message.");
 *
 */
class Logger
{
	public:
		enum Target
		{
			Stdout,
			Stderr,
			File
		};

		enum FileMode
		{
			Append,
			Truncate
		};

		enum Format
		{
			PlainText,
			Json
		};

	private:
		static const size_t INDENT_SIZE = 2;
		static const char	INDENT_CHAR = ' ';

		static std::string m_customPrefix;
		static std::string m_customColor;

		const static std::string LOG_INFO_PREFIX;
		const static std::string LOG_WARN_PREFIX;
		const static std::string LOG_ERROR_PREFIX;
		const static std::string LOG_DEBUG_PREFIX;

		static size_t		 m_indent;
		static Target		 m_target;
		static std::ofstream m_file;
		static Format		 m_format;

		static std::ostream &stream();

	public:
		static void setTarget(Target t);
		static void setFile(const std::string &path, const FileMode mode = Append);
		static void setFormat(const Format format);
		static void setCustomPrefix(const std::string &prefix);
		static void setCustomColor(const std::string &colorCode);
		static void closeFile();

		static Format	   getFormat();
		static Target	   getTarget();
		static std::string getCustomPrefix();
		static std::string getCustomColor();

		static size_t	   incrementIndent();
		static size_t	   decrementIndent();
		static size_t	   getIndent();
		static std::string indent();

		template <typename T>
		static std::string get(const T &v);
		template <typename T>
		static std::string get(T *ptr);

		static void log(const std::string &str);
		static void log(const char *cstr);
		static void log(char c);
		template <typename V>
		static void log(const V &v);
		template <typename V>
		static void log(V *ptr);


		static void info(const std::string &str);
		static void info(const char *cstr);
		static void info(char c);
		template <typename V>
		static void info(const V &v);
		template <typename V>
		static void info(V *ptr);


		static void error(const std::string &str);
		static void error(const char *cstr);
		static void error(char c);
		template <typename V>
		static void error(const V &v);
		template <typename V>
		static void error(V *ptr);


		static void warn(const std::string &str);
		static void warn(const char *cstr);
		static void warn(char c);
		template <typename V>
		static void warn(const V &v);
		template <typename V>
		static void warn(V *ptr);


		static void debug(const std::string &str);
		static void debug(const char *cstr);
		static void debug(char c);
		template <typename V>
		static void debug(const V &v);
		template <typename V>
		static void debug(V *ptr);

		static void custom(const std::string &str);
		static void custom(const char *cstr);
		static void custom(char c);
		template <typename V>
		static void custom(const V &v);
		template <typename V>
		static void custom(V *ptr);
};


#include "../srcs/Logger.tpp"

#endif
