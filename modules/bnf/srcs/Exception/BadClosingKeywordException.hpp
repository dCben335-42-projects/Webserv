#ifndef BAD_CLOSING_KEYWORD_EXCEPTION_HPP
#define BAD_CLOSING_KEYWORD_EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include "SemanticException.hpp"
#include "bnf_defs.hpp"
using namespace bnf;

class BadClosingKeywordException : public SemanticException
{
	public:
		explicit BadClosingKeywordException(const std::string &keyword) : SemanticException("Bad closing keyword: " + keyword,
																							"BadClosingKeywordException") {};
};

#endif
