#include "JsonObject.hpp"
#include "Json.hpp"

JsonObject::JsonObject()
{
}

JsonObject::JsonObject(const JsonObject &other)
{
	for (const_iterator it = other.begin(); it != other.end(); ++it)
	{
		m_members[it->first] = new Json(*it->second);
	}
}

JsonObject::~JsonObject()
{
	clear();
}

JsonObject &JsonObject::operator=(const JsonObject &other)
{
	if (this != &other)
	{
		clear();
		for (const_iterator it = other.begin(); it != other.end(); ++it)
		{
			m_members[it->first] = new Json(*it->second);
		}
	}
	return *this;
}

void JsonObject::insert(const std::string &key, Json *value)
{
	std::map<std::string, Json *>::iterator it = m_members.find(key);
	if (it != m_members.end())
	{
		delete it->second;
		it->second = value;
	}
	else
	{
		m_members[key] = value;
	}
}

Json *JsonObject::get(const std::string &key)
{
	std::map<std::string, Json *>::iterator it = m_members.find(key);
	if (it == m_members.end())
	{
		return NULL;
	}
	return it->second;
}

const Json *JsonObject::get(const std::string &key) const
{
	std::map<std::string, Json *>::const_iterator it = m_members.find(key);
	if (it == m_members.end())
	{
		return NULL;
	}
	return it->second;
}

bool JsonObject::has(const std::string &key) const
{
	return m_members.find(key) != m_members.end();
}

void JsonObject::remove(const std::string &key)
{
	std::map<std::string, Json *>::iterator it = m_members.find(key);
	if (it != m_members.end())
	{
		delete it->second;
		m_members.erase(it);
	}
}

void JsonObject::clear()
{
	for (iterator it = begin(); it != end(); ++it)
	{
		delete it->second;
	}
	m_members.clear();
}

size_t JsonObject::size() const
{
	return m_members.size();
}

std::string JsonObject::toString() const
{
	std::string result = "{";
	bool		first  = true;
	for (const_iterator it = begin(); it != end(); ++it)
	{
		if (!first)
			result += ",";
		first = false;
		result += "\"" + it->first + "\":" + it->second->toString();
	}
	result += "}";
	return result;
}

JsonObject::iterator JsonObject::begin()
{
	return m_members.begin();
}

JsonObject::iterator JsonObject::end()
{
	return m_members.end();
}

JsonObject::const_iterator JsonObject::begin() const
{
	return m_members.begin();
}

JsonObject::const_iterator JsonObject::end() const
{
	return m_members.end();
}