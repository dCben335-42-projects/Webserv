#ifndef KIT_JSON_NULL_HPP
#define KIT_JSON_NULL_HPP

#include "../AJson.hpp"

class JsonNull : public AJson
{
	public:
		JsonNull();
		std::string get() const;
};

std::ostream &operator<<(std::ostream &os, const JsonNull &null);

#endif
