#ifndef KIT_MAP_TPP
#define KIT_MAP_TPP

#include <map>
#include <string>
#include <iostream>
#include <stdexcept>
#include <set>

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

template <typename T>
void printSet(const std::set<T> &s)
{
	typename std::set<T>::const_iterator it;
	std::cout << "[";
	for (it = s.begin(); it != s.end(); ++it)
	{
		std::cout << *it << ",";
	}
	std::cout << "]" << std::endl;
}

template <typename Derived, typename K, typename V>
class MapBase
{
	public:
		/**
		 *
		 * @brief Checks if the map contains the given key.
		 *
		 * @param key The key to be checked.
		 *
		 * @return true if the key exists in the map, false otherwise.
		 *
		 */
		bool has(const K &key) const
		{
			return isInMap(static_cast<const Derived &>(*this), key);
		}


		/**
		 *
		 * @brief Gets the value associated with the given key in the map.
		 *
		 * @param key The key of the pair to be retrieved.
		 *
		 * @return The value associated with the given key.
		 *
		 */
		const V &get(const K &key) const
		{
			return getMapValue(static_cast<const Derived &>(*this), key);
		}

		V *getPtr(const K &key)
		{
			typename Derived::iterator it = static_cast<Derived &>(*this).find(key);
			if (it == static_cast<Derived &>(*this).end())
				return NULL;
			return &it->second;
		}

		/**
		 *
		 * @brief Gets the value associated with the given key in the map.
		 *
		 * @param key The key of the pair to be retrieved.
		 *
		 * @return The value associated with the given key.
		 *
		 */
		V &get(const K &key)
		{
			return getMapValue(static_cast<Derived &>(*this), key);
		}

		/**
		 *
		 * @brief Sets the value for the given key in the map. If the key does not exist, it is inserted.
		 *
		 * @param key The key of the pair to be set.
		 * @param value The value to be associated with the key.
		 *
		 */
		void set(const K &key, const V &value)
		{
			findOrInsert(static_cast<Derived &>(*this), key, value)->second = value;
		}

		/**
		 *
		 * @brief Removes the key-value pair associated with the given key from the map.
		 *
		 * @param key The key of the pair to be removed.
		 *
		 */
		void remove(const K &key)
		{
			static_cast<Derived &>(*this).erase(key);
		}


		/**
		 *
		 * @brief Tries to get the value associated with the given key.
		 *
		 * @param key The key to search for in the map.
		 * @param out A reference to a variable that will hold the value if found.
		 *
		 * @return true if the key exists in the map and out is set to the value, false otherwise.
		 *
		 */
		bool try_get(const K &key, V &out) const
		{
			typename Derived::const_iterator it = static_cast<const Derived &>(*this).find(key);
			if (it == static_cast<const Derived &>(*this).end())
				return false;
			out = it->second;
			return true;
		}

		/**
		 *
		 * @brief Tries to get a pointer to the value associated with the given key.
		 *
		 * @param key The key to search for in the map.
		 * @param out A reference to a pointer that will point to the value if found.
		 *
		 * @return true if the key exists in the map and out is set to point to the value, false otherwise.
		 *
		 */
		bool try_get(const K &key, V *&out)
		{
			typename Derived::iterator it = static_cast<Derived &>(*this).find(key);
			if (it == static_cast<Derived &>(*this).end())
				return false;
			out = &it->second;
			return true;
		}

		// /**
		//  *
		//  * @brief Returns the number of key-value pairs in the map.
		//  *
		//  * @return The size of the map.
		//  *
		//  */
		// size_t size() const
		// {
		// 	return getMapSize(static_cast<const Derived &>(*this));
		// }
};


template <typename K, typename V>
struct MapImpl
{
		typedef std::map<K, V> map_type;
};


template <typename K, typename V = void>
struct Map;


template <typename V>
struct Map<V, void>
	: public MapImpl<std::string, V>::map_type,
	  public MapBase<Map<V, void>, std::string, V>
{
		typedef typename MapImpl<std::string, V>::map_type base;

		Map() : base()
		{
		}
		Map(const base &b) : base(b)
		{
		}
		Map &operator=(const Map &other)
		{
			base::operator=(other);
			return *this;
		}
};

template <typename K, typename V>
struct Map
	: public MapImpl<K, V>::map_type,
	  public MapBase<Map<K, V>, K, V>
{
		typedef typename MapImpl<K, V>::map_type base;

		Map() : base()
		{
		}
		Map(const base &b) : base(b)
		{
		}
		Map &operator=(const Map &other)
		{
			base::operator=(other);
			return *this;
		}
		~Map()
		{
		}
};

// clang-format on

#endif
