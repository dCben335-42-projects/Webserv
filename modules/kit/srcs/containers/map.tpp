#ifndef MAP_TPP
#define MAP_TPP

#include <map>
#include <string>
#include <iostream>
#include <stdexcept>

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



template <typename Derived, typename K, typename V>
class MapBase
{
	public:
		bool has(const K &key) const
		{
			return isInMap(static_cast<const Derived &>(*this), key);
		}

		V &get(const K &key)
		{
			return getMapValue(static_cast<Derived &>(*this), key);
		}
		const V &get(const K &key) const
		{
			return getMapValue(static_cast<const Derived &>(*this), key);
		}

		void set(const K &key, const V &value)
		{
			findOrInsert(static_cast<Derived &>(*this), key, value)->second = value;
		}

		void remove(const K &key)
		{
			static_cast<Derived &>(*this).erase(key);
		}

		bool try_get(const K &key, V &out) const
		{
			typename Derived::const_iterator it = static_cast<const Derived &>(*this).find(key);
			if (it == static_cast<const Derived &>(*this).end())
				return false;
			out = it->second;
			return true;
		}

		size_t size() const
		{
			return getMapSize(static_cast<const Derived &>(*this));
		}
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
};

// clang-format on

#endif // MAP_HPP
