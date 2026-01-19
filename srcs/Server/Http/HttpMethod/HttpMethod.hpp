#ifndef HTTP_METHOD_HPP
#define HTTP_METHOD_HPP

#include <string>
#include "Map.hpp"


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
			NONE
		};


	private:
		HttpMethod::Type m_type;
		std::string		 m_strType;

		const static Map<Type, std::string> m_methodMap;
		static Map<Type, std::string>		initMethodMap();

	public:
		HttpMethod();
		HttpMethod(const std::string &method);
		HttpMethod(const Type &method);
		~HttpMethod();


		HttpMethod &operator=(const HttpMethod &other);

		Type		getType() const;
		std::string getStrType() const;



		bool operator<(const HttpMethod &other) const
		{
			return m_type < other.m_type;
		}

		bool operator==(const HttpMethod &other) const
		{
			return m_type == other.m_type;
		}

		bool operator!=(const HttpMethod &other) const
		{
			return !(*this == other);
		}
};

#endif