#include <iostream>
#include <string>
#include <cstdlib>
#include <map>
#include <sstream>
#include <cctype>

// ROT13
char rot13_char(char c)
{
	if (c >= 'a' && c <= 'z')
		return 'a' + (c - 'a' + 13) % 26;
	if (c >= 'A' && c <= 'Z')
		return 'A' + (c - 'A' + 13) % 26;
	return c;
}

std::string rot13(const std::string &s)
{
	std::string result = s;
	for (auto &c : result)
		c = rot13_char(c);
	return result;
}

// Decode URL-encoded string
std::string url_decode(const std::string &str)
{
	std::string result;
	for (size_t i = 0; i < str.size(); ++i)
	{
		if (str[i] == '+')
			result += ' ';
		else if (str[i] == '%' && i + 2 < str.size())
		{
			int				  val;
			std::stringstream ss;
			ss << std::hex << str.substr(i + 1, 2);
			ss >> val;
			result += static_cast<char>(val);
			i += 2;
		}
		else
			result += str[i];
	}
	return result;
}

// Parse form-urlencoded
std::map<std::string, std::string> parseForm(const std::string &data)
{
	std::map<std::string, std::string> result;
	std::stringstream				   ss(data);
	std::string						   pair;
	while (std::getline(ss, pair, '&'))
	{
		size_t eq = pair.find('=');
		if (eq != std::string::npos)
		{
			std::string key	  = pair.substr(0, eq);
			std::string value = pair.substr(eq + 1);
			result[key]		  = url_decode(value);
		}
	}
	return result;
}

int main()
{
	std::string contentLengthStr = std::getenv("CONTENT_LENGTH") ? std::getenv("CONTENT_LENGTH") : "0";
	int			contentLength	 = std::stoi(contentLengthStr);

	std::string postData(contentLength, '\0');
	std::cin.read(&postData[0], contentLength);

	auto		form = parseForm(postData);
	std::string text = form["text"]; // récupère seulement le champ text

	std::string transformed = rot13(text);

	// Output CGI headers
	std::cout << "Content-Type: text/plain\r\n\r\n";
	std::cout << transformed;

	return 0;
}