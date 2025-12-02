#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

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

// -----------------------------
// String utilities
// -----------------------------
bool startsWith(const std::string &s, const std::string &prefix);

// -----------------------------
// Map utilities
// -----------------------------
template <typename MapType, typename KeyType, typename ValueType>
typename MapType::iterator getOrInsert(MapType &map, const KeyType &key, const ValueType &value);

template <typename MapType, typename KeyType>
bool isInMap(const MapType &m, const KeyType &key);

template <typename T>
void printMapKeys(const Map<T>& m);

// -----------------------------
// Container utilities
// -----------------------------
template <typename ContainerType, typename ValueType>
bool isInContainer(const ContainerType &c, const ValueType &value);

// -----------------------------
// Logging / debug utilities
// -----------------------------
template <typename T>
void log(const T &v);

template <typename T>
void log(T *ptr);

template <typename T>
void log(const std::vector<T>& vec);

template <typename K, typename V>
void log(const std::map<K,V>& m);

void log(const char *s);
void log(char *s);

#include "utils.tpp"

#endif
