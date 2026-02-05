#ifndef KIT_JSON_BOOL_HPP
#define KIT_JSON_BOOL_HPP

#include "../AJson.hpp"

class JsonBool : public AJson
{
	private:
		bool m_value;

	public:
		JsonBool(const bool value);
		bool get() const;
};

std::ostream &operator<<(std::ostream &os, const JsonBool &b);

#endif