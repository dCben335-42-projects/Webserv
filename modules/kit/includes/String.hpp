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
};




#endif