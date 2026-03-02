#ifndef BNF_RECURSIVE_LOOP_CONCEPT_EXCEPTION_HPP
#define BNF_RECURSIVE_LOOP_CONCEPT_EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include "../SemanticException.hpp"
#include "bnf_defs.hpp"
using namespace bnf;

class RecursiveLoopConceptException : public SemanticException
{
	public:
		explicit RecursiveLoopConceptException() :
			SemanticException(
				"Recursive loop concept detected",
				"RecursiveLoopConceptException") {};
};

#endif
