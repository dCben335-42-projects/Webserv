#ifndef JSON_VECTOR_HPP
#define JSON_VECTOR_HPP

#include <vector>
#include <string>
#include "Json_fwd.hpp"

class JsonVector
{
	private:
		std::vector<Json *> m_elements;

	public:
		JsonVector();
		JsonVector(const JsonVector &other);
		~JsonVector();

		JsonVector &operator=(const JsonVector &other);

		void		push_back(Json *element);
		Json	   *operator[](size_t index);
		const Json *operator[](size_t index) const;
		size_t		size() const;
		void		clear();

		std::string toString() const;
};

#endif