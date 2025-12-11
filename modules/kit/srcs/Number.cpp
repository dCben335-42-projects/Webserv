#include "Number.hpp"
#include <sstream>
#include <cstdlib>

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

int Number::toInt(const std::string &str)
{
	return std::atoi(str.c_str());
}