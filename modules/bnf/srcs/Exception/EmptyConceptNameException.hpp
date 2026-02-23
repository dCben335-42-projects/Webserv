#ifndef BNF_EMPTY_CONCEPT_NAME_EXCEPTION_HPP
#define BNF_EMPTY_CONCEPT_NAME_EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include "SemanticException.hpp"
#include "bnf_defs.hpp"
using namespace bnf;

class EmptyConceptNameException : public SemanticException
{
	public:
		explicit EmptyConceptNameException() : SemanticException("A concept name must be provided "
																 "before the rule separator '" +
																	 EQRULE + "' or '" + ORRULE + "'. ",
																 "EmptyConceptNameException") {};
};

#endif
