std::string joinTokens(const Vec<std::string> &tokens, size_t start);

template <typename T>
void setDirective(T &contextObj, const std::string &key, const Vec<std::string> &tokens)
{
	if (key == "add_header" && tokens.size() >= 2)
		contextObj.setAddHeadersAt(tokens[0], joinTokens(tokens, 1));
	else if (key == "cgi_pass" && !tokens.empty())
		contextObj.setCgiPassAt(tokens[0], joinTokens(tokens, 1));
	else if (key == "error_page" && !tokens.empty())
		contextObj.setErrorPagesAt(tokens[0], joinTokens(tokens, 1));
	else
		contextObj.setDirectiveAt(key, tokens);
}