#include <string>

/**
 * @brief Checks if a string starts with a given prefix.
 *
 * This function compares the beginning of the input string `s`
 * with the provided `prefix` and returns true if they match.
 *
 * @param s The string to check.
 * @param prefix The prefix to compare against the start of `s`.
 * @return true if `s` starts with `prefix`.
 * @return false if `s` is shorter than `prefix` or the prefix does not match.
 *
 * @example
 * std::string text = "Hello World";
 * bool result = startsWith(text, "Hello"); // returns true
 * bool result2 = startsWith(text, "World"); // returns false
 *
 * @note This function is case-sensitive.
 */
bool startsWith(const std::string &s, const std::string &prefix)
{
    if (s.size() < prefix.size())
        return false;
    return s.compare(0, prefix.size(), prefix) == 0;
}

/**
 * @brief Inserts a (key, value) pair into a map if the key does not exist.
 *
 * This is a generic function that works with any standard map
 * (std::map or std::multimap). It returns an iterator to the existing
 * or newly inserted element.
 *
 * @tparam Map The type of the map (e.g., std::map<Key, Value>)
 * @tparam Key The type of the key
 * @tparam Value The type of the value
 * @param map Reference to the map
 * @param key The key to insert or search for
 * @param value The value to insert if the key does not exist
 * @return typename Map::iterator Iterator to the element
 *
 * @note If the key already exists, the value is not modified.
 *
 * @example
 * std::map<std::string, int> m;
 * auto it = get_or_insert(m, std::string("a"), 42);
 */
template <typename Map, typename Key, typename Value>
typename Map::iterator getOrInsert(Map &map, const Key &key, const Value &value)
{
    return map.insert(typename Map::value_type(key, value)).first;
}

/**
 * @brief Checks if a key exists in a map.
 *
 * Generic function compatible with any standard map.
 *
 * @tparam MapType The type of the map (e.g., std::map<Key, Value>)
 * @tparam KeyType The type of the key
 * @param m Const reference to the map
 * @param key The key to search for
 * @return true if the key exists
 * @return false otherwise
 *
 * @example
 * std::map<std::string, int> m;
 * m["a"] = 42;
 * bool exists = is_in_map(m, std::string("a")); // returns true
 */
template <typename MapType, typename KeyType>
bool isInMap(const MapType &m, const KeyType &key)
{
    return m.find(key) != m.end();
}

/**
 * @brief Checks if an element exists in a container.
 * 
 * This is a generic function compatible with any container that
 * provides `begin()` and `end()` iterators, and whose elements
 * can be compared using the `==` operator.
 * 
 * @tparam ContainerType The type of the container (e.g., std::vector<T>, std::list<T>, std::set<T>)
 * @tparam ValueType The type of the value to search for (must be comparable with the container's elements)
 * @param c Const reference to the container
 * @param value The value to search for
 * @return true if the value is present in the container
 * @return false otherwise
 * 
 * @note This function works with:
 *   - std::vector<T>
 *   - std::list<T>
 *   - std::deque<T>
 *   - std::set<T> / std::multiset<T>
 * 
 * @note This function does not work directly with std::map or std::multimap,
 *       because their elements are key-value pairs.
 */
template <typename ContainerType, typename ValueType>
bool isInContainer(const ContainerType &c, const ValueType &value)
{
    return std::find(c.begin(), c.end(), value) != c.end();
}

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