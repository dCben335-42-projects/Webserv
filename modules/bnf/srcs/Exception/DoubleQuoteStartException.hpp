#ifndef BNF_DOUBLE_QUOTE_START_EXCEPTION_HPP
#define BNF_DOUBLE_QUOTE_START_EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include "SemanticException.hpp"
#include "bnf_defs.hpp"
using namespace bnf;

class DoubleQuoteStartException : public SemanticException
{
	public:
		explicit DoubleQuoteStartException() : SemanticException("A token cannot begin with double quotes",
																 "DoubleQuoteStartException") {};
};

#endif
