
#ifndef KIT_JSON_STRING_HPP
#define KIT_JSON_STRING_HPP

#include "../AJson.hpp"

class JsonString : public AJson
{
	private:
		std::string m_value;

	public:
		JsonString(const std::string &value);
		const std::string &getValue() const;
};

std::ostream &operator<<(std::ostream &os, const JsonString &str);

#endif