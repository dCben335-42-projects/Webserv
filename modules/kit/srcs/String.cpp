#include "String.hpp"

String::String()
{
}

String::~String()
{
}

std::string String::toUpper(const std::string &str)
{
	std::string result = str;
	for (size_t i = 0; i < result.size(); ++i)
		result[i] = std::toupper(static_cast<unsigned char>(result[i]));
	return result;
}

std::string String::toLower(const std::string &str)
{
	std::string result = str;
	for (size_t i = 0; i < result.size(); ++i)
		result[i] = std::tolower(static_cast<unsigned char>(result[i]));
	return result;
}

std::string String::trim(const std::string &str)
{
	size_t start = 0;
	while (start < str.size() && std::isspace(static_cast<unsigned char>(str[start])))
		++start;
	size_t end = str.size();
	while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1])))
		--end;
	return str.substr(start, end - start);
}

std::string String::htmlEscape(const std::string &input)
{
	std::string result;
	result.reserve(input.size() * 1.1);
	for (std::string::size_type i = 0; i < input.size(); ++i)
	{
		char c = input[i];
		switch (c)
		{
			case '&':
				result += "&amp;";
				break;
			case '<':
				result += "&lt;";
				break;
			case '>':
				result += "&gt;";
				break;
			case '"':
				result += "&quot;";
				break;
			case '\'':
				result += "&#39;";
				break;
			default:
				result += c;
				break;
		}
	}
	return result;
}