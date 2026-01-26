#ifndef HTTP_METHOD_HPP
#define HTTP_METHOD_HPP

#include "Map.hpp"

#define M_GET "GET"
#define M_POST "POST"
#define M_DELETE "DELETE"
#define M_PUT "PUT"
#define M_PATCH "PATCH"

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
		const static Map<Type, std::string> m_methodMap;
		std::string							m_strType;
		HttpMethod::Type					m_type;

		static Map<Type, std::string> initMethodMap();

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
			std::cerr << "Comparing " << m_strType << " and " << other.m_strType << std::endl;
			return m_type == other.m_type;
		}

		bool operator!=(const HttpMethod &other) const
		{
			std::cerr << "Comparing " << m_strType << " and " << other.m_strType << std::endl;
			return !(*this == other);
		}
};

#endif
