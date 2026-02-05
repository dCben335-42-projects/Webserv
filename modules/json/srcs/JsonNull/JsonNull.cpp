#include "JsonNull.hpp"

JsonNull::JsonNull() :
	AJson(AJson::NIL)
{
}

std::string JsonNull::get() const
{
	return "null";
}

std::ostream &operator<<(std::ostream &os, const JsonNull &null)
{
	os << null.get();
	return os;
}