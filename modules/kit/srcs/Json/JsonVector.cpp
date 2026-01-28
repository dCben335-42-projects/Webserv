#include "JsonVector.hpp"
#include "Json.hpp"
#include <stdexcept>

JsonVector::JsonVector()
{
}

JsonVector::JsonVector(const JsonVector &other)
{
	for (size_t i = 0; i < other.m_elements.size(); ++i)
	{
		m_elements.push_back(new Json(*other.m_elements[i]));
	}
}

JsonVector::~JsonVector()
{
	clear();
}

JsonVector &JsonVector::operator=(const JsonVector &other)
{
	if (this != &other)
	{
		clear();
		for (size_t i = 0; i < other.m_elements.size(); ++i)
		{
			m_elements.push_back(new Json(*other.m_elements[i]));
		}
	}
	return *this;
}

void JsonVector::push_back(Json *element)
{
	m_elements.push_back(element);
}

Json *JsonVector::operator[](size_t index)
{
	if (index >= m_elements.size())
	{
		throw std::out_of_range("JsonVector index out of range");
	}
	return m_elements[index];
}

const Json *JsonVector::operator[](size_t index) const
{
	if (index >= m_elements.size())
	{
		throw std::out_of_range("JsonVector index out of range");
	}
	return m_elements[index];
}

size_t JsonVector::size() const
{
	return m_elements.size();
}

void JsonVector::clear()
{
	for (size_t i = 0; i < m_elements.size(); ++i)
	{
		delete m_elements[i];
	}
	m_elements.clear();
}

std::string JsonVector::toString() const
{
	std::string result = "[";
	for (size_t i = 0; i < m_elements.size(); ++i)
	{
		if (i > 0)
			result += ",";
		result += m_elements[i]->toString();
	}
	result += "]";
	return result;
}