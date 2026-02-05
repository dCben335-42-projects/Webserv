#include "Json.hpp"

AJson::AJson(const Type type) : m_type(type)
{
}

AJson::~AJson()
{
}

/**
 *
 * @brief Gets the type of the JSON value.
 *
 * @see AJson::Type
 *
 * @return The type of the JSON value.
 *
 */
AJson::Type AJson::getType() const
{
	return m_type;
}


/**
 *
 * @brief Creates a new JSON array.
 *
 * @return A pointer to the newly created JSON array.
 */
JsonArray *AJson::createArray()
{
	return new JsonArray();
}
/**
 *
 * @brief Creates a new JSON array with the given elements.
 *
 * @param elements The elements to be included in the array.
 *
 * @return A pointer to the newly created JSON array.
 *
 */
JsonArray *AJson::createArray(const Vec<AJson *> &elements)
{
	return new JsonArray(elements);
}
/**
 *
 * @brief Creates a new JSON object.
 *
 * @return A pointer to the newly created JSON object.
 *
 */
JsonObject *AJson::createObject()
{
	return new JsonObject();
}
/**
 *
 * @brief Creates a new JSON object with the given members.
 *
 * @param members The members to be included in the object.
 *
 * @return A pointer to the newly created JSON object.
 *
 */
JsonObject *AJson::createObject(const Map<std::string, AJson *> &members)
{
	return new JsonObject(members);
}
/**
 *
 * @brief Creates a new JSON string with the given value.
 *
 * @param value The value of the JSON string.
 *
 * @return A pointer to the newly created JSON string.
 *
 */
JsonString *AJson::createString(const std::string &value)
{
	return new JsonString(value);
}
/**
 *
 * @brief Creates a new JSON number with the given value.
 *
 * @param value The value of the JSON number.
 *
 * @return A pointer to the newly created JSON number.
 *
 */
JsonNumber *AJson::createNumber(double value)
{
	return new JsonNumber(value);
}
/**
 *
 * @brief Creates a new JSON boolean with the given value.
 *
 * @param value The value of the JSON boolean.
 *
 * @return A pointer to the newly created JSON boolean.
 *
 */
JsonBool *AJson::createBool(bool value)
{
	return new JsonBool(value);
}
/**
 *
 * @brief Creates a new JSON null value.
 *
 * @return A pointer to the newly created JSON null value.
 *
 */
JsonNull *AJson::createNull()
{
	return new JsonNull();
}




/**
 *
 * @brief Casts the JSON value to a JSON object if it is of the correct type.
 *
 * @return A pointer to the JSON object if the type is correct, NULL otherwise.
 *
 */
JsonArray *AJson::asArray()
{
	return m_type == AJson::ARRAY ? static_cast<JsonArray *>(this) : NULL;
}
/**
 *
 * @brief Casts the JSON value to a JSON object if it is of the correct type.
 *
 * @return A pointer to the JSON object if the type is correct, NULL otherwise.
 *
 */
const JsonArray *AJson::asArray() const
{
	return m_type == AJson::ARRAY ? static_cast<const JsonArray *>(this) : NULL;
}


/**
 *
 * @brief Casts the JSON value to a JSON object if it is of the correct type.
 *
 * @return A pointer to the JSON object if the type is correct, NULL otherwise.
 *
 */
JsonObject *AJson::asObject()
{
	return m_type == AJson::OBJECT ? static_cast<JsonObject *>(this) : NULL;
}
/**
 *
 * @brief Casts the JSON value to a JSON object if it is of the correct type.
 *
 * @return A pointer to the JSON object if the type is correct, NULL otherwise.
 *
 */
const JsonObject *AJson::asObject() const
{
	return m_type == AJson::OBJECT ? static_cast<const JsonObject *>(this) : NULL;
}

/**
 *
 * @brief Casts the JSON value to a JSON string if it is of the correct type.
 *
 * @return A pointer to the JSON string if the type is correct, NULL otherwise.
 *
 */
JsonString *AJson::asString()
{
	return m_type == AJson::STRING ? static_cast<JsonString *>(this) : NULL;
}
/**
 *
 * @brief Casts the JSON value to a JSON string if it is of the correct type.
 *
 * @return A pointer to the JSON string if the type is correct, NULL otherwise.
 *
 */
const JsonString *AJson::asString() const
{
	return m_type == AJson::STRING ? static_cast<const JsonString *>(this) : NULL;
}

/**
 *
 * @brief Casts the JSON value to a JSON number if it is of the correct type.
 *
 * @return A pointer to the JSON number if the type is correct, NULL otherwise.
 *
 */
JsonNumber *AJson::asNumber()
{
	return m_type == AJson::NUMBER ? static_cast<JsonNumber *>(this) : NULL;
}
/**
 *
 * @brief Casts the JSON value to a JSON number if it is of the correct type.
 *
 * @return A pointer to the JSON number if the type is correct, NULL otherwise.
 *
 */
const JsonNumber *AJson::asNumber() const
{
	return m_type == AJson::NUMBER ? static_cast<const JsonNumber *>(this) : NULL;
}

/**
 *
 * @brief Casts the JSON value to a JSON boolean if it is of the correct type.
 *
 * @return A pointer to the JSON boolean if the type is correct, NULL otherwise.
 *
 */
JsonBool *AJson::asBool()
{
	return m_type == AJson::BOOL ? static_cast<JsonBool *>(this) : NULL;
}
/**
 *
 * @brief Casts the JSON value to a JSON boolean if it is of the correct type.
 *
 * @return A pointer to the JSON boolean if the type is correct, NULL otherwise.
 *
 */
const JsonBool *AJson::asBool() const
{
	return m_type == AJson::BOOL ? static_cast<const JsonBool *>(this) : NULL;
}

/**
 *
 * @brief Casts the JSON value to a JSON null if it is of the correct type.
 *
 * @return A pointer to the JSON null if the type is correct, NULL otherwise.
 *
 */
JsonNull *AJson::asNull()
{
	return m_type == AJson::NIL ? static_cast<JsonNull *>(this) : NULL;
}
/**
 *
 * @brief Casts the JSON value to a JSON null if it is of the correct type.
 *
 * @return A pointer to the JSON null if the type is correct, NULL otherwise.
 *
 */
const JsonNull *AJson::asNull() const
{
	return m_type == AJson::NIL ? static_cast<const JsonNull *>(this) : NULL;
}



#include "Logger.hpp"

std::ostream &operator<<(std::ostream &os, const AJson &json)
{
	switch (json.getType())
	{
		case AJson::STRING:
			os << Logger::get(json.asString()->get());
			return os;
		case AJson::NUMBER:
			os << Logger::get(json.asNumber()->get());
			return os;
		case AJson::BOOL:
			os << Logger::get(json.asBool()->get());
			return os;
		case AJson::ARRAY:
			os << Logger::get(json.asArray()->get());
			return os;
		case AJson::OBJECT:
			os << Logger::get(json.asObject()->get());
			return os;
		case AJson::NIL:
			os << "null";
			return os;
	}
	os << Logger::get(json);
	return os;
}