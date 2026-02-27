#include "Number.hpp"
#include <sstream>
#include <cstdlib>
#include <cctype>
#include <iomanip>

Number::Number()
{
}
Number::~Number()
{
}

std::string Number::toString(const int number)
{
	std::ostringstream oss;
	oss << number;
	return oss.str();
}

std::string Number::toString(const unsigned int number)
{
	std::ostringstream oss;
	oss << number;
	return oss.str();
}

std::string Number::toString(const size_t number)
{
	std::ostringstream oss;
	oss << number;
	return oss.str();
}

std::string Number::toString(const long number)
{
	std::ostringstream oss;
	oss << number;
	return oss.str();
}

int Number::fromString(const std::string &str)
{
	return std::atoi(str.c_str());
}

bool Number::isNumber(const std::string &str)
{
	if (str.empty())
	{
		return false;
	}
	for (size_t i = 0; i < str.size(); ++i)
	{
		if (!std::isdigit(static_cast<unsigned char>(str[i])))
		{
			return false;
		}
	}
	return true;
}

size_t Number::toSizeT(const std::string &str)
{
	return static_cast<size_t>(std::atoi(str.c_str()));
}

unsigned short Number::toUShort(const std::string &str)
{
	return static_cast<unsigned short>(std::strtoul(str.c_str(), NULL, 10));
}

int Number::toInt(const std::string &str)
{
	return static_cast<int>(std::strtol(str.c_str(), NULL, 10));
}

unsigned long Number::toULong(const std::string &str)
{
	return static_cast<unsigned long>(std::strtoul(str.c_str(), NULL, 10));
}

std::string Number::bytesToHumanSize(const size_t bytes)
{
	const char *sizes[] = {"B", "KB", "MB", "GB", "TB", "PB"};
	size_t		order	= 0;
	double		size	= static_cast<double>(bytes);

	while (size >= 1024 && order < 5)
	{
		size /= 1024;
		++order;
	}

	std::ostringstream oss;
	oss << std::fixed << std::setprecision(2) << size << " " << sizes[order];
	return oss.str();
}

int Number::hexToInt(char c)
{
	if (c <= '9')
		return c - '0';
	if (c <= 'F')
		return c - 'A' + 10;
	return c - 'a' + 10;
}