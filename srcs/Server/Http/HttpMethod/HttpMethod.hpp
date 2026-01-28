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
		const static Map<Type, std::string> m_methodMap;
		std::string							m_strType;
		HttpMethod::Type					m_type;

		static Map<Type, std::string> initMethodMap();

	public:
		HttpMethod();
		HttpMethod(const std::string &method);
		HttpMethod(const Type &method);
		~HttpMethod();

		bool operator<(const HttpMethod &other) const;
		HttpMethod &operator=(const HttpMethod &other);

		Type		getType() const;
		std::string getStrType() const;
};

#endif
