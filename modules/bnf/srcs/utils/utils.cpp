#include "utils.hpp"
#include "bnf_defs.hpp"

bool startsWith(const std::string &s, const std::string &prefix)
{
	if (s.size() < prefix.size())
		return false;
	return s.compare(0, prefix.size(), prefix) == 0;
}

bool matchClass(char c, const char *cls)
{
	for (size_t i = 0; cls[i]; ++i)
	{
		if (cls[i] == '-' && i > 0 && cls[i + 1])
		{
			char start = cls[i - 1];
			char end   = cls[i + 1];
			if (c >= start && c <= end)
				return true;
		}
		else if (c == cls[i])
		{
			return true;
		}
	}
	return false;
}


bool is_alpha_string(const std::string &s)
{
	if (s.empty())
		return false;
	for (size_t i = 0; i < s.size(); ++i)
		if (!std::isalpha(s[i]))
			return false;
	return true;
}

bool validate_string_strict(const std::string &s)
{
	size_t len = s.size();
	if (s == bnf::CURSOR)
		return true;
	if (len < 2)
		return false;

	if (s[0] == '\'' && s[len - 1] == '\'')
		return true;

	for (size_t i = 0; i < len; ++i)
		if (!std::isalpha(s[i]) && s[i] != '_')
			return false;

	return true;
}

bool isOnlyWhitespace(const std::string &str, size_t start)
{
	if (start >= str.size())
		return false;
	for (std::string::const_iterator it = str.begin() + start; it != str.end(); ++it)
	{
		if (!isspace(static_cast<unsigned char>(*it)))
		{
			return false;
		}
	}
	return true;
}

std::string erasePaternWrapper(std::string &s, const std::string pattern)
{
	return s.erase(0, s.find_first_not_of(pattern)).erase(s.find_last_not_of(pattern) + 1);
}