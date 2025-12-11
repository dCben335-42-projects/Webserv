#ifndef LOG_HPP
#define LOG_HPP

#include <cstddef>
#include <string>
#include <iostream>

#define SPACE ' '
#define TAP_SIZE 2

class Log
{
	private:
		static size_t m_indent;

	public:
		static size_t	   incrementIndent();
		static size_t	   decrementIndent();
		static size_t	   getIndent();
		static std::string indent();
};

void		log(const char *s);
std::string getLogJson(const std::string &s);

#endif