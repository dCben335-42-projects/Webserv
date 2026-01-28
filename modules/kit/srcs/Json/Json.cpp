#include "Json.hpp"
#include <sstream>
#include <stdexcept>

// Private methods
void Json::cleanup()
{
	if (m_str)
	{
		delete m_str;
		m_str = NULL;
	}
	if (m_vec)
	{
		delete m_vec;
		m_vec = NULL;
	}
	if (m_obj)
	{
		delete m_obj;
		m_obj = NULL;
	}
}

void Json::copy(const Json &other)
{
	cleanup();
	type_	 = other.type_;
	m_bool	 = other.m_bool;
	m_number = other.m_number;

	if (other.m_str)
	{
		m_str = new JsonString(*other.m_str);
	}
	if (other.m_vec)
	{
		m_vec = new JsonVector(*other.m_vec);
	}
	if (other.m_obj)
	{
		m_obj = new JsonObject(*other.m_obj);
	}
}

// Constructors
Json::Json() :
	type_(TYPE_NULL),
	m_bool(false),
	m_number(0.0),
	m_str(NULL),
	m_vec(NULL),
	m_obj(NULL)
{
}

Json::Json(Type type) :
	type_(type),
	m_bool(false),
	m_number(0.0),
	m_str(NULL),
	m_vec(NULL),
	m_obj(NULL)
{
	switch (type)
	{
		case TYPE_STRING:
			m_str = new JsonString();
			break;
		case TYPE_ARRAY:
			m_vec = new JsonVector();
			break;
		case TYPE_OBJECT:
			m_obj = new JsonObject();
			break;
		default:
			break;
	}
}

Json::Json(bool value) :
	type_(TYPE_BOOL),
	m_bool(value),
	m_number(0.0),
	m_str(NULL),
	m_vec(NULL),
	m_obj(NULL)
{
}

Json::Json(int value) :
	type_(TYPE_NUMBER),
	m_bool(false),
	m_number(value),
	m_str(NULL),
	m_vec(NULL),
	m_obj(NULL)
{
}

Json::Json(double value) :
	type_(TYPE_NUMBER),
	m_bool(false),
	m_number(value),
	m_str(NULL),
	m_vec(NULL),
	m_obj(NULL)
{
}

Json::Json(const std::string &value) :
	type_(TYPE_STRING),
	m_bool(false),
	m_number(0.0),
	m_str(new JsonString(value)),
	m_vec(NULL),
	m_obj(NULL)
{
}

Json::Json(const char *value) :
	type_(TYPE_STRING),
	m_bool(false),
	m_number(0.0),
	m_str(new JsonString(value)),
	m_vec(NULL),
	m_obj(NULL)
{
}

Json::Json(const Json &other) :
	type_(TYPE_NULL),
	m_bool(false),
	m_number(0.0),
	m_str(NULL),
	m_vec(NULL),
	m_obj(NULL)
{
	copy(other);
}

Json::~Json()
{
	cleanup();
}

// Assignment
Json &Json::operator=(const Json &other)
{
	if (this != &other)
	{
		copy(other);
	}
	return *this;
}

// Type checking
Json::Type Json::getType() const
{
	return type_;
}

bool Json::isNull() const
{
	return type_ == TYPE_NULL;
}

bool Json::isBool() const
{
	return type_ == TYPE_BOOL;
}

bool Json::isNumber() const
{
	return type_ == TYPE_NUMBER;
}

bool Json::isString() const
{
	return type_ == TYPE_STRING;
}

bool Json::isArray() const
{
	return type_ == TYPE_ARRAY;
}

bool Json::isObject() const
{
	return type_ == TYPE_OBJECT;
}

// Value getters
bool Json::asBool() const
{
	if (type_ != TYPE_BOOL)
	{
		throw std::runtime_error("Json value is not a boolean");
	}
	return m_bool;
}

double Json::asNumber() const
{
	if (type_ != TYPE_NUMBER)
	{
		throw std::runtime_error("Json value is not a number");
	}
	return m_number;
}

int Json::asInt() const
{
	return static_cast<int>(asNumber());
}

std::string Json::asString() const
{
	if (type_ != TYPE_STRING)
	{
		throw std::runtime_error("Json value is not a string");
	}
	return m_str->getValue();
}

JsonVector &Json::asArray()
{
	if (type_ != TYPE_ARRAY)
	{
		throw std::runtime_error("Json value is not an array");
	}
	return *m_vec;
}

const JsonVector &Json::asArray() const
{
	if (type_ != TYPE_ARRAY)
	{
		throw std::runtime_error("Json value is not an array");
	}
	return *m_vec;
}

JsonObject &Json::asObject()
{
	if (type_ != TYPE_OBJECT)
	{
		throw std::runtime_error("Json value is not an object");
	}
	return *m_obj;
}

const JsonObject &Json::asObject() const
{
	if (type_ != TYPE_OBJECT)
	{
		throw std::runtime_error("Json value is not an object");
	}
	return *m_obj;
}

// Serialization
std::string Json::toString() const
{
	std::ostringstream oss;

	switch (type_)
	{
		case TYPE_NULL:
			return "null";
		case TYPE_BOOL:
			return m_bool ? "true" : "false";
		case TYPE_NUMBER:
			oss << m_number;
			return oss.str();
		case TYPE_STRING:
			return m_str->toString();
		case TYPE_ARRAY:
			return m_vec->toString();
		case TYPE_OBJECT:
			return m_obj->toString();
		default:
			return "null";
	}
}

// Static factory methods
Json *Json::createNull()
{
	return new Json();
}

Json *Json::createBool(bool value)
{
	return new Json(value);
}

Json *Json::createNumber(double value)
{
	return new Json(value);
}

Json *Json::createString(const std::string &value)
{
	return new Json(value);
}

Json *Json::createArray()
{
	return new Json(TYPE_ARRAY);
}

Json *Json::createObject()
{
	return new Json(TYPE_OBJECT);
}