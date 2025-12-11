#ifndef MAP_TPP
#define MAP_TPP

#include <map>
#include <string>
#include <iostream>

// template <typename K, typename V>
// struct MapImpl {
//     typedef std::map<K, V> type;
//     typedef typename type::iterator iterator;
//     typedef typename type::const_iterator const_iterator;
// };

// // Single Map type
// template <typename K, typename V = void>
// struct Map;

// template <typename V>
// struct Map<V, void> : public MapImpl<std::string, V> {
//     typedef typename MapImpl<std::string, V>::type type;
//     typedef typename type::iterator iterator;
//     typedef typename type::const_iterator const_iterator;
// };

// template <typename K, typename V>
// struct Map : public MapImpl<K, V> {
//     typedef typename MapImpl<K, V>::type type;
//     typedef typename type::iterator iterator;
//     typedef typename type::const_iterator const_iterator;
// };

// MapImpl: produces the real std::map<K,V>
template <typename K, typename V>
struct MapImpl {
    typedef std::map<K, V> map_type;
};

template <typename K, typename V = void>
struct Map;

template <typename V>
struct Map<V, void> : public MapImpl<std::string, V>::map_type
{
    typedef typename MapImpl<std::string, V>::map_type base;

    Map() : base() {}
    Map(const base &b) : base(b) {}
};

template <typename K, typename V>
struct Map : public MapImpl<K, V>::map_type
{
    typedef typename MapImpl<K, V>::map_type base;

    Map() : base() {}
    Map(const base &b) : base(b) {}
};

template <typename M, typename K, typename V>
typename M::iterator findOrInsert(M &map, const K &key, const V &value)
{
    return map.insert(typename M::value_type(key, value)).first;
}

template <typename M, typename K>
bool isInMap(const M &m, const K &key)
{
    return m.find(key) != m.end();
}

template <typename M, typename K>
typename M::mapped_type &getMapValue(M &m, const K &key)
{
    return m.find(key)->second;
}

template <typename M, typename K>
const typename M::mapped_type &getMapValue(const M &m, const K &key)
{
    return m.find(key)->second;
}

template <typename M>
size_t getMapSize(const M &m)
{
    return m.size();
}





// ANCHOR -- Print functions --

template <typename M, typename K>
void printMapKey(const M &m, const K &key)
{
    typename M::const_iterator it = m.find(key);
    if (it != m.end())
        std::cout << it->first;
    else
        std::cout << "Key not found";
}

template <typename M, typename K>
void printMapValue(const M &m, const K &key)
{
    typename M::const_iterator it = m.find(key);
    if (it != m.end())
        std::cout << it->second;
    else
        std::cout << "Key not found";
}

template <typename M, typename K, typename V>
void printMapEntry(const M &m, const K &key)
{
    typename M::const_iterator it = m.find(key);
    if (it != m.end())
        std::cout << it->first << ": " << it->second;
    else
        std::cout << "Key not found";
}

template <typename M>
void printMap(const M &m)
{
    for (typename M::const_iterator it = m.begin(); it != m.end(); ++it)
    {
        printMapEntry(m, it->first);
        std::cout << "\n";
    }
    std::cout << std::flush;
}

template <typename M>
void printMapKeys(const M &m)
{
    for (typename M::const_iterator it = m.begin(); it != m.end(); ++it)
    {
        printMapKeys(m, it->first);
        std::cout << "\n";
    }
    std::cout << std::flush;
}

template <typename M>
void printMapValues(const M &m)
{
    for (typename M::const_iterator it = m.begin(); it != m.end(); ++it)
    {
        printMapValue(m, it->first);
        std::cout << "\n";
    }
    std::cout << std::flush;
}

#endif