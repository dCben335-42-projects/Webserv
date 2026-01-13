#ifndef STRING_HPP
#define STRING_HPP

#include <cstring>
#include <string>
#include <cctype>


class String
{
	private:
		String();
		~String();

	public:
		static std::string toUpper(const std::string &str);
		static std::string toLower(const std::string &str);

		static std::string trim(const std::string &str);
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
};




#endif