#ifndef HTTP_METHOD_HPP
#define HTTP_METHOD_HPP

#include "Map.hpp"


#define M_GET "GET"
#define M_POST "POST"
#define M_DELETE "DELETE"
#define M_PUT "PUT"
#define M_PATCH "PATCH"
#define M_HEAD "HEAD"
#define M_OPTIONS "OPTIONS"
#define M_TRACE "TRACE"
#define M_CONNECT "CONNECT"

#include <Set.hpp>

class HttpMethod
{
	public:
		enum Type
		{
			GET,
			POST,
			DELETE,
			PUT,
			PATCH,
			HEAD,
			OPTIONS,
			TRACE,
			CONNECT,
			NONE
		};

	private:
		std::string		 m_str;
		HttpMethod::Type m_type;

		static Map<Type, std::string>		initMethodMap();
		static const Map<Type, std::string> m_methodMap;

	public:
		HttpMethod();
		HttpMethod(const std::string &method);
		HttpMethod(const Type &method);
		~HttpMethod();

		bool		operator<(const HttpMethod &other) const;
		HttpMethod &operator=(const HttpMethod &other);

		Type		getType() const;
		std::string getStrType() const;

		static Set<HttpMethod> getAllMethods();
		static std::string	   toString(const Set<HttpMethod> &methods);
};

std::ostream &operator<<(std::ostream &os, const HttpMethod &method);


#endif
