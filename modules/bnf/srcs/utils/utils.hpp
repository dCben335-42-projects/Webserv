#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

// -----------------------------
// Type helpers for template iterators
// -----------------------------
template <typename T>
struct Vec {
    typedef std::vector<T> type;
    typedef typename type::iterator iterator;
    typedef typename type::const_iterator const_iterator;
};

template <typename T>
struct Map {
    typedef std::map<std::string, T> type;
    typedef typename type::iterator iterator;
    typedef typename type::const_iterator const_iterator;
};

// template <typename S, typename T>
// struct Map {
//     typedef std::map<S, T> type;
//     typedef typename type::iterator iterator;
//     typedef typename type::const_iterator const_iterator;
// };

// -----------------------------
// String utilities
// -----------------------------
bool startsWith(const std::string &s, const std::string &prefix);

bool miniRegex(const std::string& s, const char* pattern);

bool matchClass(char c, const char* cls);

bool is_alpha_string(const std::string& s);

bool validate_string_strict(const std::string& s);

bool isOnlyWhitespace(const std::string& str, size_t start);

#endif
