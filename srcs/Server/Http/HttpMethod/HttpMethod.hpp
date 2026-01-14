#ifndef HTTP_METHOD_HPP
#define HTTP_METHOD_HPP

#include <string>
#include "Map.hpp"

enum EMethodType
{
	GET,
	POST,
	DELETE,
	PUT,
	PATCH,
	NONE
};

class HttpMethod
{
	private:
		EMethodType m_type;
		std::string m_strType;

		const static Map<EMethodType, std::string> m_methodMap;

		static Map<EMethodType, std::string> initMethodMap();

	public:
		HttpMethod();
		explicit HttpMethod(const std::string &method);
		explicit HttpMethod(const EMethodType &method);
		~HttpMethod();

		HttpMethod &operator=(const HttpMethod &other);

		EMethodType getType() const;
		std::string getStrType() const;
};

#endif