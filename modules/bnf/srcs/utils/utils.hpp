#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>
#include <map>
#include <vector>

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

bool startsWith(const std::string &s, const std::string &prefix);

template <typename Map, typename Key, typename Value>
typename Map::iterator getOrInsert(Map &map, const Key &key, const Value &value);

template <typename MapType, typename KeyType>
bool isInMap(const MapType &m, const KeyType &key);

template <typename T>
void printMapKeys(const Map<T>& m);

template<typename T>
void log(const T &v);

template<typename T>
void log(T *ptr);

template<typename T>
void log(const std::vector<T>&);

template<typename K, typename V>
void log(const std::map<K,V>&);

void log(char *s);

void log(const char *s);

template <typename T>
void log(const std::vector<T> &vec);

template<typename K, typename V>
void log(const std::map<K, V> &m);

template <typename ContainerType, typename ValueType>
bool isInContainer(const ContainerType &c, const ValueType &value);

#endif