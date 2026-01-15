#ifndef LOG_JSON_HPP
#define LOG_JSON_HPP

#include <string>
#include <sstream>
#include "Map.hpp"
#include "Vector.hpp"
#include <set>

/**
 *
 * @brief Utility class to convert values to JSON-like strings for logging
 *
 * @tparam V The type of the value to be converted
 *
 * @return A string representing the JSON format of the value
 *
 */
class LoggerJson
{
	private:
		template <typename MapType>
		static std::string mapGet(const MapType &m);

		template <typename VecType>
		static std::string vectorGet(const VecType &vec);

		template <typename SetType>
		static std::string setGet(const SetType &set);

		template <typename SetType>
		static std::string setGetAddress(const SetType &set);

	public:
		template <typename V>
		static std::string get(const V &v);
		template <typename V>
		static std::string get(V *ptr);

		template <typename V>
		static std::string get(const std::vector<V> &vec);
		template <typename V>
		static std::string get(const Vec<V> &vec);

		template <typename K, typename V>
		static std::string get(const std::map<K, V> &m);
		template <typename K, typename V>
		static std::string get(const Map<K, V> &m);

		template <typename V>
		static std::string get(const std::set<V> &set);
		template <typename V>
		static std::string get(const Set<V> &set);
		template <typename V>
		static std::string get(const V &v, const bool getAddress);
};

#include "../srcs/LoggerJson.tpp"

#endif
