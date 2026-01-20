#ifndef LOG_JSON_HPP
#define LOG_JSON_HPP

#include <string>
#include <sstream>
#include "Map.hpp"
#include "Vec.hpp"
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
		static std::string mapGet(const MapType &m, const bool getAddress = false);

		template <typename VecType>
		static std::string vectorGet(const VecType &vec, const bool getAddress = false);

		template <typename SetType>
		static std::string setGet(const SetType &set, const bool getAddress = false);

	public:
		template <typename V>
		static std::string get(const V &v, const bool getAddress = false);

		template <typename V>
		static std::string get(V *ptr, const bool getAddress = false);
		template <typename V>
		static std::string get(V const *ptr, const bool getAddress = false);

		template <typename V>
		static std::string get(const std::vector<V> &vec, const bool getAddress = false);
		template <typename V>
		static std::string get(const Vec<V> &vec, const bool getAddress = false);

		template <typename K, typename V>
		static std::string get(const std::map<K, V> &m, const bool getAddress = false);
		template <typename K, typename V>
		static std::string get(const Map<K, V> &m, const bool getAddress = false);

		template <typename V>
		static std::string get(const std::set<V> &set, const bool getAddress = false);
		template <typename V>
		static std::string get(const Set<V> &set, const bool getAddress = false);
};

#include "../srcs/LoggerJson.tpp"

#endif
