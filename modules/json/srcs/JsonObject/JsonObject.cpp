#include "Json.hpp"
#include "Logger.hpp"

JsonObject::JsonObject() :
	AJson(AJson::OBJECT)
{
}

JsonObject::JsonObject(const Map<std::string, AJson *> &members) :
	AJson(AJson::OBJECT),
	m_members(members)
{
}

JsonObject::~JsonObject()
{
	for (iterator it = m_members.begin(); it != m_members.end(); ++it)
		delete it->second;
}


/**
 *
 * @brief Gets the members of the JSON object.
 *
 * @return A reference to the map containing the members of the JSON object.
 *
 * */
const Map<std::string, AJson *> &JsonObject::get() const
{
	return m_members;
}
/**
 *
 * @brief Gets the value associated with the given key in the JSON object.
 *
 * @param key The key of the pair to be retrieved.
 *
 * @return The value associated with the given key,
 * or NULL if the key does not exist in the JSON object.
 *
 * */
const AJson *JsonObject::get(const std::string &key) const
{
	return m_members.has(key) ? m_members.get(key) : NULL;
}


/**
 *
 * @brief Gets the value associated with the given key in the JSON object.
 *
 * @param key The key of the pair to be retrieved.
 *
 * @return The value associated with the given key,
 * or NULL if the key does not exist in the JSON object.
 *
 * */
AJson *JsonObject::get(const std::string &key)
{
	return m_members.has(key) ? m_members.get(key) : NULL;
}

/**
 *
 * @brief Sets the value for the given key in the JSON object.
 * If the key does not exist, it is inserted.
 *
 * @param key The key of the pair to be set.
 * @param value The value to be associated with the key.
 *
 * */
void JsonObject::set(const std::string &key, AJson *value)
{
	m_members.set(key, value);
}

/**
 *
 * @brief Removes the key-value pair associated with the given key from the JSON object.
 *
 * @param key The key of the pair to be removed.
 *
 * */
void JsonObject::remove(const std::string &key)
{
	m_members.remove(key);
}


/**
 *
 * @brief Creates a new JSON string value and sets it for the given key in the JSON object.
 *
 * @param key The key for which the JSON string value is to be created and set.
 * @param value The value of the JSON string to be created and set for the given key
 *
 * */
void JsonObject::create(const std::string &key, const char *value)
{
	set(key, createString(value));
}

/**
 *
 * @brief Creates a new JSON string value and sets it for the given key in the JSON object.
 *
 * @param key The key for which the JSON string value is to be created and set.
 * @param value The value of the JSON string to be created and set for the given key
 *
 * */
void JsonObject::create(const std::string &key, const std::string &value)
{
	set(key, createString(value));
}

/**
 *
 * @brief Creates a new JSON number value and sets it for the given key in the JSON object.
 *
 * @param key The key for which the JSON number value is to be created and set.
 * @param value The value of the JSON number to be created and set for the given key
 *
 * */
void JsonObject::create(const std::string &key, const double value)
{
	set(key, createNumber(value));
}

/**
 *
 * @brief Creates a new JSON boolean value and sets it for the given key in the JSON object.
 *
 * @param key The key for which the JSON boolean value is to be created and set.
 * @param value The value of the JSON boolean to be created and set for the given key
 *
 * */
void JsonObject::create(const std::string &key, const bool value)
{
	set(key, createBool(value));
}

/**
 *
 * @brief Creates a new JSON array value and sets it for the given key in the JSON object.
 *
 * @param key The key for which the JSON array value is to be created and set.
 * @param array The elements of the JSON array to be created and set for the given key
 *
 * */
void JsonObject::create(const std::string &key, const Vec<AJson *> &array)
{
	set(key, createArray(array));
}

/**
 *
 * @brief Creates a new JSON object value and sets it for the given key in the JSON object.
 *
 * @param key The key for which the JSON object value is to be created and set.
 * @param object The members of the JSON object to be created and set for the given key
 *
 * */
void JsonObject::create(const std::string &key, const Map<std::string, AJson *> &object)
{
	set(key, createObject(object));
}




std::ostream &operator<<(std::ostream &os, const JsonObject &obj)
{
	os << Logger::get(obj.get());
	return os;
}