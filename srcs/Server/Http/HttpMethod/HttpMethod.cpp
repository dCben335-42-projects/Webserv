#include "HttpMethod.hpp"
#include "httpExceptions.hpp"

const Map<HttpMethod::Type, std::string> HttpMethod::m_methodMap = HttpMethod::initMethodMap();

Map<HttpMethod::Type, std::string> HttpMethod::initMethodMap()
{
	Map<Type, std::string> methodMap;

	methodMap[GET]	  = M_GET;
	methodMap[POST]	  = M_POST;
	methodMap[DELETE] = M_DELETE;
	methodMap[PUT]	  = M_PUT;
	methodMap[PATCH]  = M_PATCH;
	return methodMap;
}

HttpMethod::HttpMethod() : m_type(NONE)
{
}



/**
 *
 * @brief Constructor for HttpMethod from string.
 *
 * @param method The HTTP method as a string.
 *
 * @throws Http501Exception if the method is not recognized.
 *
 */
HttpMethod::HttpMethod(const std::string &method)
{
	Map<Type, std::string>::const_iterator it  = m_methodMap.begin();
	Map<Type, std::string>::const_iterator end = m_methodMap.end();

	while (it != end)
	{
		if (it->second == method)
		{
			m_type = it->first;
			m_str  = it->second;
			return;
		}
		++it;
	}

	throw Http501Exception(method);
}


/**
 *
 * @brief Constructor for HttpMethod from Type enum.
 *
 * @param method The HTTP method type.
 *
 * @note It initializes the HttpMethod object with the corresponding string representation.
 *
 */
HttpMethod::HttpMethod(const Type &method)
{
	Map<Type, std::string>::const_iterator it = m_methodMap.find(method);

	if (it != m_methodMap.end())
	{
		m_type = it->first;
		m_str  = it->second;
		return;
	}
}

/**
 *
 * @brief Assignment operator for HttpMethod.
 *
 * @param other The other HttpMethod object to assign from.
 *
 * @note It only copies the type and string representation.
 *
 * @return A reference to this HttpMethod object.
 *
 */
HttpMethod &HttpMethod::operator=(const HttpMethod &other)
{
	if (this == &other)
		return *this;
	m_type = other.m_type;
	m_str  = other.m_str;
	return *this;
}

/**
 *
 * @brief Compares two HttpMethod objects.
 *
 * @param other The other HttpMethod object to compare with.
 *
 * @note It is used by Set to order HttpMethod objects.
 *
 * @return true if this HttpMethod is less than the other, false otherwise.
 *
 */
bool HttpMethod::operator<(const HttpMethod &other) const
{
	return m_type < other.m_type;
}



HttpMethod::~HttpMethod()
{
}

/**
 *
 * @brief Gets the HTTP method type.
 *
 * @return The HTTP method type.
 *
 */
HttpMethod::Type HttpMethod::getType() const
{
	return m_type;
}

/**
 *
 * @brief Gets the string representation of the HTTP method.
 *
 * @return The HTTP method as a string.
 *
 */
std::string HttpMethod::getStrType() const
{
	return m_str;
}


/**
 *
 * @brief Gets a set of all defined HTTP methods.
 *
 * @return A set containing all HttpMethod objects defined in the method map.
 *
 */
Set<HttpMethod> HttpMethod::getAllMethods()
{
	Set<HttpMethod>						   methods;
	Map<Type, std::string>::const_iterator it  = m_methodMap.begin();
	Map<Type, std::string>::const_iterator end = m_methodMap.end();

	while (it != end)
	{
		methods.push(HttpMethod(it->first));
		++it;
	}

	return methods;
}

/**
 *
 * @brief Converts a set of HttpMethod objects to a comma-separated string.
 *
 * @param methods The set of HttpMethod objects to convert.
 *
 * @return A string containing the comma-separated HTTP method names.
 *
 */
std::string HttpMethod::toString(const Set<HttpMethod> &methods)
{
	std::string						result;
	Set<HttpMethod>::const_iterator end = methods.end();
	Set<HttpMethod>::const_iterator it	= methods.begin();

	while (it != end)
	{
		result += it->getStrType();
		++it;
		if (it != end)
		{
			result += ", ";
		}
	}

	return result;
}



std::ostream &operator<<(std::ostream &os, const HttpMethod &method)
{
	os << method.getStrType() << " (" << method.getType() << ")";
	return os;
}