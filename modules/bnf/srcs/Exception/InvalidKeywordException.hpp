#ifndef INVALID_KEYWORD_EXCEPTION_HPP
#define INVALID_KEYWORD_EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include "SemanticException.hpp"
#include "bnf_defs.hpp"
using namespace bnf;

class InvalidKeywordException : public SemanticException
{
	public:
		explicit InvalidKeywordException(const std::string &keyword) : SemanticException("Invalid keyword: " + keyword,
																						 "InvalidKeywordException") {};
};

#endif
