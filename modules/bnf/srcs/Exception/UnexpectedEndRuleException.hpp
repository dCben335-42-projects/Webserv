#ifndef UNEXPECTED_END_RULE_EXCEPTION_HPP
#define UNEXPECTED_END_RULE_EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include "SemanticException.hpp"
#include "bnf_defs.hpp"
using namespace bnf;

class UnexpectedEndRuleException : public SemanticException
{
	public:
		explicit UnexpectedEndRuleException(const std::string &rule) : SemanticException("Unexpected end of rule: " + rule,
																						 "UnexpectedEndRuleException") {};
};

#endif
