#ifndef PARSER_IPP
#define PARSER_IPP

#include <string>
#include <Vec.hpp>
#include "Number.hpp"


template <typename T, typename DirectiveType>
bool Parser::applyStringDirective(T &obj, const DirectiveType &directive, const std::string &value)
{
	(obj.*directive.setString)(value);
	return true;
}

template <typename T, typename DirectiveType>
bool Parser::applyBoolDirective(T &obj, const DirectiveType &directive, const std::string &value)
{
	if (value == "on" || value == "true" || value == "1")
		(obj.*directive.setBool)(true);
	else if (value == "off" || value == "false" || value == "0")
		(obj.*directive.setBool)(false);
	else
		throw std::invalid_argument("Invalid boolean value: " + value);
	return true;
}

template <typename T, typename DirectiveType>
bool Parser::applyArrayDirective(T &obj, const DirectiveType &directive, const Vec<std::string> &values)
{
	(obj.*directive.setArray)(values);
	return true;
}

template <typename T, typename DirectiveType>
bool Parser::applyNumberDirective(T &obj, const DirectiveType &directive, const std::string &value)
{
	(obj.*directive.setNumber)(Number::toSizeT(value));
	return true;
}

template <typename T, typename DirectiveType>
bool Parser::applyDirective(
	T					   &obj,
	const std::string	   &key,
	const Vec<std::string> &values,
	const DirectiveType		directives[],
	size_t					numDirectives)
{
	for (size_t i = 0; i < numDirectives; ++i)
	{
		const DirectiveType &directive = directives[i];
		if (key != directive.key)
			continue;
		if (directive.setString)
			return applyStringDirective(obj, directive, values[0]);
		if (directive.setBool)
			return applyBoolDirective(obj, directive, values[0]);
		if (directive.setArray)
			return applyArrayDirective(obj, directive, values);
		if (directive.setNumber)
			return applyNumberDirective(obj, directive, values[0]);
		return true;
	}
	return false;
}


#endif