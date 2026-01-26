#include "String.hpp"

String::String()
{
}

String::~String()
{
}

/**
 *
 * @brief Converts a string to uppercase.
 *
 * @param str The input string to be converted.
 *
 * @return A new string with all characters converted to uppercase.
 *
 */
std::string String::toUpper(const std::string &str)
{
	std::string result = str;
	for (size_t i = 0; i < result.size(); ++i)
		result[i] = std::toupper(static_cast<unsigned char>(result[i]));
	return result;
}

/**
 *
 * @brief Converts a string to lowercase.
 *
 * @param str The input string to be converted.
 *
 * @return A new string with all characters converted to lowercase.
 *
 */
std::string String::toLower(const std::string &str)
{
	std::string result = str;
	for (size_t i = 0; i < result.size(); ++i)
		result[i] = std::tolower(static_cast<unsigned char>(result[i]));
	return result;
}


/**
 *
 * @brief Trims leading and trailing whitespace from a string.
 *
 * @param str The input string to be trimmed.
 *
 * @note Whitespace characters are determined using std::isspace, so
 * it includes spaces, tabs, newlines, and other standard whitespace characters.
 *
 * @return A new string with leading and trailing whitespace removed.
 *
 */
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


/**
 *
 * @brief Escapes special HTML characters in a string.
 *
 * @param input The input string to be escaped.
 *
 * @note This function replaces the following characters:
 *       & -> &amp;
 *       < -> &lt;
 *       > -> &gt;
 *       " -> &quot;
 *       ' -> &#39;
 * 	 	\n -> <br/>
 *
 * @note This is useful for preventing HTML injection
 *
 * @return A new string with special HTML characters replaced
 * by their corresponding HTML entities.
 */
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
			case '\n':
				result += "<br/>";
				break;
			default:
				result += c;
				break;
		}
	}
	return result;
}

/**
 *
 * @brief Replaces all occurrences of a substring within a string with another substring.
 *
 * @param str The string to perform replacements on.
 * @param from The substring to be replaced.
 * @param to The substring to replace with.
 *
 * @note If the 'from' substring is empty, the function does nothing.
 *
 * @note This function modifies the input string in place.
 *
 */
void String::replaceAll(
	std::string		  &str,
	const std::string &from,
	const std::string &to)
{
	size_t startPos = 0;
	while ((startPos = str.find(from, startPos)) != std::string::npos)
	{
		str.replace(startPos, from.length(), to);
		startPos += to.length();
	}
}

/**
 *
 * @brief Replaces all occurrences of a substring within a string with another substring.
 *
 * @param str The string to perform replacements on.
 * @param from The substring to be replaced.
 * @param to The substring to replace with.
 *
 * @note If the 'from' substring is empty, the function does nothing.
 *
 * @return A new string with all occurrences of 'from' replaced by 'to'.
 *
 */
std::string String::replaceAll(
	const std::string &str,
	const std::string &from,
	const std::string &to)
{
	std::string result	 = str;
	size_t		startPos = 0;
	while ((startPos = result.find(from, startPos)) != std::string::npos)
	{
		result.replace(startPos, from.length(), to);
		startPos += to.length();
	}
	return result;
}
/**
 * @brief Removes duplicate slashes from a file path.
 *
 *@param path The input file path.
 *
 *@ return A new string with consecutive slashes replaced by a single slash.
 *
 */
std::string String::removeDuplicateSlashes(const std::string &path)
{
	std::string result;
	result.reserve(path.size());
	bool wasSlash = false;

	for (size_t i = 0; i < path.size(); ++i)
	{
		char c = path[i];
		if (c == '/')
		{
			if (!wasSlash)
			{
				result += c;
				wasSlash = true;
			}
		}
		else
		{
			result += c;
			wasSlash = false;
		}
	}
	return result;
}
#include <iostream>
/**
 *
 * @brief Splits a string into tokens based on specified delimiters.
 *
 * @param str The input string to be split.
 * @param delimiters A C-string containing delimiter characters.
 *
 * @return A vector of strings, each representing a token from the input string.
 *
 */
Vec<std::string> String::split(const std::string &str, const char *delimiters)
{
	Vec<std::string> result;
	size_t			 start = 0;
	size_t			 end   = 0;
	std::string		 token;

	while (end != std::string::npos)
	{
		end	  = str.find_first_of(delimiters, start);
		token = str.substr(start, end - start);
		if (!token.empty())
			result.push_back(token);
		start = (end == std::string::npos) ? end : end + 1;
	}
	return result;
}

Vec<std::string> String::splitPattern(const std::string &str, const std::string &pattern)
{
	Vec<std::string> result;
	size_t			 start = 0;
	size_t			 end   = 0;
	std::string		 token;
	const size_t	 boundaryLen = pattern.size();

	while (end != std::string::npos)
	{
		end	  = str.find(pattern, start);
		token = str.substr(start, end - start);
		if (!token.empty())
			result.push_back(token);
		start = end + boundaryLen;
	}
	return result;
}