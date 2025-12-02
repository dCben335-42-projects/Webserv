#ifndef UTILS_TPP
#define UTILS_TPP

#include "./utils.hpp"

// -----------------------------
// getOrInsert
// -----------------------------
template <typename MapType, typename KeyType, typename ValueType>
typename MapType::iterator getOrInsert(MapType &map, const KeyType &key, const ValueType &value)
{
    return map.insert(typename MapType::value_type(key, value)).first;
}

// -----------------------------
// isInMap
// -----------------------------
template <typename MapType, typename KeyType>
bool isInMap(const MapType &m, const KeyType &key)
{
    return m.find(key) != m.end();
}

// -----------------------------
// printMapKeys
// -----------------------------
template <typename T>
void printMapKeys(const Map<T>& m)
{
    for (typename Map<T>::const_iterator it = m.begin(); it != m.end(); ++it)
        std::cout << it->first << std::endl;
}

// -----------------------------
// log()
// recursive templates for pointers, vector, map
// -----------------------------
template<typename T>
void log(const T &v)
{
    std::cout << v;
}

template<typename T>
void log(T *ptr)
{
    if(ptr)
        log(*ptr);  // recursive call
    else
        std::cout << "null";
}

template <typename T>
void log(const std::vector<T> &vec)
{
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i)
    {
        log(vec[i]);
        if (i + 1 < vec.size())
            std::cout << ", ";
    }
    std::cout << "]";
}

template<typename K, typename V>
void log(const std::map<K,V>& m)
{
    std::cout << "{\n";
    typename std::map<K,V>::const_iterator it = m.begin();
    typename std::map<K,V>::const_iterator end = m.end();
    while(it != end)
    {
        std::cout << "\t";

        log(it->first);
        std::cout << ": ";
        log(it->second);
        if (++it != end) std::cout << ",\n";
    }
    std::cout << "\n}";
}

// -----------------------------
// isInContainer
// -----------------------------
template <typename ContainerType, typename ValueType>
bool isInContainer(const ContainerType &c, const ValueType &value)
{
    return std::find(c.begin(), c.end(), value) != c.end();
}

#endif // UTILS_TPP
