#include "Number.hpp"
#include <sstream>
#include <cstdlib>
#include <cctype>
#include <iomanip>
#include <cmath>

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


/**
 *
 * @brief Converts a human-readable file size string into its equivalent size in bytes.
 *
 * @param str The human-readable file size string to convert.
 * - The string should consist of a numeric value followed by an optional unit (k, m, g for kilobytes, megabytes, gigabytes respectively).
 *
 * @return The equivalent size in bytes as a size_t value.
 *
 */
size_t Number::toBytes(const std::string &str)
{
	size_t order = 0;
	double size	 = 0.0;
	size_t i	 = 0;

	while (i < str.size() && (std::isdigit(static_cast<unsigned char>(str[i])) || str[i] == '.'))
	{
		size = size * 10 + (str[i] - '0');
		++i;
	}
	if (i < str.size())
	{
		char unit = std::tolower(static_cast<unsigned char>(str[i]));
		switch (unit)
		{
			case 'k':
				order = 1;
				break;
			case 'm':
				order = 2;
				break;
			case 'g':
				order = 3;
				break;
			default:
				order = 0;
				break;
		}
	}
	return static_cast<size_t>(size * std::pow(1024, order));
}

int Number::hexToInt(char c)
{
	if (c <= '9')
		return c - '0';
	if (c <= 'F')
		return c - 'A' + 10;
	return c - 'a' + 10;
}