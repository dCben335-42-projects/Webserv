#include "Json.hpp"
#include "Logger.hpp"

JsonArray::JsonArray() :
	AJson(AJson::ARRAY)
{
	m_elements = Vec<AJson *>();
}

JsonArray::JsonArray(const Vec<AJson *> &elements) :
	AJson(AJson::ARRAY),
	m_elements(elements)
{
}

JsonArray::~JsonArray()
{
	for (size_t i = 0; i < m_elements.size(); ++i)
		delete m_elements[i];
}

/**
 *
 * @brief Gets the elements of the array.
 *
 * @return A reference to the vector containing the elements of the array.
 *
 */
const Vec<AJson *> &JsonArray::get() const
{
	return m_elements;
}

/**
 *
 * @brief Gets the value at the given index in the array.
 *
 * @param index The index of the value to be retrieved.
 *
 * @return The value at the given index, or NULL if the index is out of bounds.
 *
 */
AJson *JsonArray::get(size_t index) const
{
	return m_elements.has(index) ? m_elements.get(index) : NULL;
}


/**
 *
 * @brief Gets the last value in the array.
 *
 * @return The last value in the array, or NULL if the array is empty.
 *
 */
AJson *JsonArray::getLast() const
{
	return m_elements.size() > 0 ? m_elements.get(m_elements.size() - 1) : NULL;
}

/**
 *
 * @brief Checks if the array contains a value at the given index.
 *
 * @param index The index to be checked.
 *
 * @return true if the index exists in the array, false otherwise.
 *
 */
bool JsonArray::has(size_t index) const
{
	return m_elements.has(index);
}

/**
 *
 * @brief Adds a value to the end of the array.
 *
 * @param value The value to be added.
 *
 */
void JsonArray::push(AJson *value)
{
	m_elements.push(value);
}

/**
 *
 * @brief Removes the value at the given index from the array.
 *
 * @param index The index of the value to be removed.
 *
 */
void JsonArray::remove(size_t index)
{
	m_elements.remove(index);
}

/**
 *
 * @brief Checks if the array contains the given value.
 *
 * @param value The value to be checked.
 *
 * @return true if the value exists in the array, false otherwise.
 *
 */
bool JsonArray::contains(AJson *value) const
{
	return m_elements.contains(value);
}

/**
 *
 * @brief Returns the number of elements in the array.
 *
 * @return The size of the array.
 *
 */
size_t JsonArray::size() const
{
	return m_elements.size();
}




/**
 *
 * @brief Creates a new JSON string with the given value and adds it to the array.
 *
 * @param value The value of the JSON string to be created and added to the array.
 *
 */
void JsonArray::create(const char *value)
{
	m_elements.push(createString(value));
}

/**
 *
 * @brief Creates a new JSON string with the given value and adds it to the array.
 *
 * @param value The value of the JSON string to be created and added to the array.
 *
 */
void JsonArray::create(const std::string &value)
{
	m_elements.push(createString(value));
}
/**
 *
 * @brief Creates a new JSON number with the given value and adds it to the array.
 *
 * @param value The value of the JSON number to be created and added to the array.
 *
 */
void JsonArray::create(double value)
{
	m_elements.push(createNumber(value));
}
/**
 *
 * @brief Creates a new JSON boolean with the given value and adds it to the array.
 *
 * @param value The value of the JSON boolean to be created and added to the array.
 *
 */
void JsonArray::create(bool value)
{
	m_elements.push(createBool(value));
}
/**
 *
 * @brief Creates a new JSON array with the given elements and adds it to the array.
 *
 * @param array The elements of the JSON array to be created and added to the array.
 *
 */
void JsonArray::create(const Vec<AJson *> &array)
{
	m_elements.push(createArray(array));
}
/**
 *
 * @brief Creates a new JSON object with the given members and adds it to the array.
 *
 * @param object The members of the JSON object to be created and added to the array.
 *
 */
void JsonArray::create(const Map<std::string, AJson *> &object)
{
	m_elements.push(createObject(object));
}




std::ostream &operator<<(std::ostream &os, const JsonArray &arr)
{
	os << Logger::get(arr.get());
	return os;
}