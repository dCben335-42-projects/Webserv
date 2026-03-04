#ifndef BNF_UTILS_HPP
#define BNF_UTILS_HPP

#include <string>

bool validateStringStrict(const std::string &s);

bool isOnlyWhitespace(const std::string &str, size_t start);

std::string erasePaternWrapper(std::string &s, const std::string pattern);

#endif
