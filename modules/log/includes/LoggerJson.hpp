#ifndef LOG_LOGGER_JSON_HPP
#define LOG_LOGGER_JSON_HPP

#include <string>
#include "Map.hpp"
#include "Vec.hpp"
#include "Set.hpp"


/**
 *
 * @brief Utility class to convert values to JSON-like strings for logging
 *
 * @note This class provides static methods to convert various data types,
 * including primitive types, pointers, vectors, maps, and sets, into JSON-like
 * string representations. It supports an option to log either the content of
 * the values or their memory addresses.
 *
 * @usage LoggerJson::get(value, getAddress);
 *
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
