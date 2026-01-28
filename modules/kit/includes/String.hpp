#ifndef KIT_STRING_HPP
#define KIT_STRING_HPP

#include <cstring>
#include <string>
#include <cctype>
#include "Vec.hpp"

class String
{
	private:
		String();
		~String();

	public:
		static std::string toUpper(const std::string &str);
		static std::string toLower(const std::string &str);

		static std::string trim(const std::string &str);
		static std::string trimBegin(const std::string &str);

		static std::string htmlEscape(const std::string &input);

		static void replaceAll(
			std::string		  &str,
			const std::string &from,
			const std::string &to);

		static std::string replaceAll(
			const std::string &str,
			const std::string &from,
			const std::string &to);

		static std::string removeDuplicateSlashes(const std::string &path);

		static Vec<std::string> split(const std::string &str, const char *delimiters);
		static Vec<std::string> splitPattern(const std::string &str, const std::string &pattern);
};

#endif
