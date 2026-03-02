#ifndef BNF_BAD_PREFIX_EXCEPTION_HPP
#define BNF_BAD_PREFIX_EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include "../SemanticException.hpp"
#include "bnf_defs.hpp"
using namespace bnf;

class BadPrefixException : public SemanticException
{
	public:
		explicit BadPrefixException() :
			SemanticException(
				"A prefix must be a positive integer followed by '>'",
				"BadPrefixException") {};
};

#endif
