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
	for (Map<std::string, AJson *>::iterator it = m_members.begin(); it != m_members.end(); ++it)
		delete it->second;
}



const Map<std::string, AJson *> &JsonObject::get() const
{
	return m_members;
}
const AJson *JsonObject::get(const std::string &key) const
{
	return m_members.has(key) ? m_members.get(key) : NULL;
}




void JsonObject::set(const std::string &key, AJson *value)
{
	m_members.set(key, value);
}

void JsonObject::remove(const std::string &key)
{
	m_members.remove(key);
}





void JsonObject::create(const std::string &key, const std::string &value)
{
	set(key, createString(value));
}

void JsonObject::create(const std::string &key, const double value)
{
	set(key, createNumber(value));
}

void JsonObject::create(const std::string &key, const bool value)
{
	set(key, createBool(value));
}

void JsonObject::create(const std::string &key, const Vec<AJson *> &array)
{
	set(key, createArray(array));
}


void JsonObject::create(const std::string &key, const Map<std::string, AJson *> &object)
{
	set(key, createObject(object));
}




std::ostream &operator<<(std::ostream &os, const JsonObject &obj)
{
	os << Logger::get(obj.get());
	return os;
}