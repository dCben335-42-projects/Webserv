#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

bool startsWith(const std::string &s, const std::string &prefix);

bool miniRegex(const std::string& s, const char* pattern);

bool matchClass(char c, const char* cls);

bool is_alpha_string(const std::string& s);

bool validate_string_strict(const std::string& s);

bool isOnlyWhitespace(const std::string& str, size_t start);

std::string erasePaternWrapper(std::string &s, const std::string pattern);

#endif
