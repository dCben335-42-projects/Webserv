#include <string>
#include <map>
#include <vector>

std::string joinTokens(const std::vector<std::string> &tokens, size_t start);

template <typename T>
void setDirective(T &contextObj, const std::string &key, const std::vector<std::string> &tokens)
{
	if (key == "add_header" && tokens.size() >= 2)
	{
		contextObj.add_headers[tokens[0]] = joinTokens(tokens, 1);
	}
	else if (key == "cgi_pass" && !tokens.empty())
	{
		contextObj.cgi_pass[tokens[0]] = joinTokens(tokens, 1);
	}
	else if (key == "error_page" && !tokens.empty())
	{
		contextObj.error_pages[tokens[0]] = joinTokens(tokens, 1);
	}
	else
	{
		contextObj.directives[key] = tokens;
	}
}