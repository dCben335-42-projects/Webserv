#include "utils.hpp"
#include "bnf_defs.hpp"

bool validateStringStrict(const std::string &s)
{
	size_t len = s.size();
	if (s == bnf::CURSOR)
		return true;
	if (len < 2)
		return false;

	if (s[0] == '\'' && s[len - 1] == '\'')
		return true;

	for (size_t i = 0; i < len; ++i)
		if (!std::isalpha(static_cast<unsigned char>(s[i])) && s[i] != '_')
			return false;

	return true;
}

bool isOnlyWhitespace(const std::string &str, size_t start)
{
	if (start >= str.size())
		return false;
	for (std::string::const_iterator it = str.begin() + start; it != str.end(); ++it)
	{
		if (!std::isspace(static_cast<unsigned char>(*it)))
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