#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include <string>
#include <Map.hpp>
#include <Vector.hpp>
#include <Set.hpp>



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
			Truncate,
		};

	private:
		static const size_t INDENT_SIZE = 2;
		static const char	INDENT_CHAR = ' ';

		static size_t		 m_indent;
		static Target		 m_target;
		static std::ofstream m_file;

		static std::ostream &stream();

	public:
		static void setTarget(Target t);
		static void setFile(const std::string &path, const FileMode mode = Append);
		static void closeFile();

		static size_t	   incrementIndent();
		static size_t	   decrementIndent();
		static size_t	   getIndent();
		static std::string indent();

		static void log(const std::string &str);
		static void log(const char *cstr);
		static void log(char c);

		template <typename V>
		static void log(const V &v);

		template <typename V>
		static void log(V *ptr);

		template <typename V>
		static void log(V const *ptr);

		template <typename V>
		static void log(const std::vector<V> &vec);
		template <typename V>
		static void log(const Vec<V> &vec);

		template <typename K, typename V>
		static void log(const std::map<K, V> &m);
		template <typename K, typename V>
		static void log(const Map<K, V> &m);

		template <typename V>
		static void log(const std::set<V> &set);
		template <typename V>
		static void log(const Set<V> &set);
};


#include "../srcs/Logger.tpp"

#endif