#ifndef KIT_STRING_HPP
#define KIT_STRING_HPP

#include <cstring>
#include <string>
#include <cctype>
#include "Vec.hpp"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

class String
{
	private:
		String();
		~String();

	public:
		static std::string toUpper(const std::string &str);
		static std::string toLower(const std::string &str);

		static bool		   isNotTrimmed(const std::string &str);
		static std::string trim(const std::string &str);
		static std::string trimBegin(const std::string &str);
		static std::string trimEnd(const std::string &str);


		static void replaceAll(
			std::string		  &str,
			const std::string &from,
			const std::string &to);

		static std::string replaceAll(
			const std::string &str,
			const std::string &from,
			const std::string &to);

		static std::string removeDuplicateSlashes(const std::string &path);
		static std::string urlDecode(const std::string &str);

		static Vec<std::string> split(const std::string &str, const char *delimiters);
		static Vec<std::string> splitPattern(const std::string &str, const std::string &pattern);

		static bool match(const std::string &pattern, const std::string &str);

		static bool	  equalsIgnoreCase(char a, char b);
		static size_t findIC(const std::string &str, const std::string &substr);
		static size_t rfindIC(const std::string &str, const std::string &substr);


		static std::string htmlEscape(const std::string &input);
		static std::string htmlUnescape(const std::string &input);

		static std::string escape(const std::string &input);
		static std::string unescape(const std::string &input);

		static std::string normalizePath(const std::string &path);

		static bool startWith(const std::string &str, const std::string &prefix);
		static bool endWith(const std::string &str, const std::string &suffix);
};

#endif
