#include "JsonNull.hpp"

JsonNull::JsonNull() :
	AJson(AJson::NIL)
{
}

std::ostream &operator<<(std::ostream &os, const JsonNull &null)
{
	(void) null;
	os << "null";
	return os;
}