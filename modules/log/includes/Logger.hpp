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

		static size_t m_indent;
		static Target m_target;
		static Format m_format;
		static Logger m_instance;


		static std::ofstream					*m_file;
		static Map<std::string, std::ofstream *> m_openFiles;

		typedef Map<std::string, std::ofstream *>::iterator FileIt;

		static std::ostream &stream();

	public:
		static Logger &setTarget(Target t);
		static Logger &setFile(const std::string &path, const FileMode mode = Append, const std::string &key = "");
		static Logger &setFormat(const Format format);
		static Logger &setCustomPrefix(const std::string &prefix);
		static Logger &setCustomColor(const std::string &colorCode);
		static Logger &closeFile(const std::string &key);
		static Logger &closeFiles();

		static Format	   getFormat();
		static Target	   getTarget();
		static std::string getCustomPrefix();
		static std::string getCustomColor();

		static Logger	  &incrementIndent();
		static Logger	  &decrementIndent();
		static size_t	   getIndent();
		static std::string indent();

		template <typename T>
		static std::string get(const T &v);
		template <typename T>
		static std::string get(T *ptr);

		static Logger &log(const std::string &str);
		static Logger &log(const char *cstr);
		static Logger &log(char c);
		template <typename V>
		static Logger &log(const V &v);
		template <typename V>
		static Logger &log(V *ptr);


		static Logger &info(const std::string &str);
		static Logger &info(const char *cstr);
		static Logger &info(char c);
		template <typename V>
		static Logger &info(const V &v);
		template <typename V>
		static Logger &info(V *ptr);


		static Logger &error(const std::string &str);
		static Logger &error(const char *cstr);
		static Logger &error(char c);
		template <typename V>
		static Logger &error(const V &v);
		template <typename V>
		static Logger &error(V *ptr);


		static Logger &warn(const std::string &str);
		static Logger &warn(const char *cstr);
		static Logger &warn(char c);
		template <typename V>
		static Logger &warn(const V &v);
		template <typename V>
		static Logger &warn(V *ptr);


		static Logger &debug(const std::string &str);
		static Logger &debug(const char *cstr);
		static Logger &debug(char c);
		template <typename V>
		static Logger &debug(const V &v);
		template <typename V>
		static Logger &debug(V *ptr);

		static Logger &custom(const std::string &str);
		static Logger &custom(const char *cstr);
		static Logger &custom(char c);
		template <typename V>
		static Logger &custom(const V &v);
		template <typename V>
		static Logger &custom(V *ptr);
};


#include "../srcs/Logger.tpp"

#endif
