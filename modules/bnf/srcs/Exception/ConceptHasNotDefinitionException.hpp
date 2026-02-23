#ifndef BNF_CONCEPT_HAS_NOT_DEFINITION_EXCEPTION_HPP
#define BNF_CONCEPT_HAS_NOT_DEFINITION_EXCEPTION_HPP

#include <string>
#include <stdexcept>
#include "SemanticException.hpp"
#include "bnf_defs.hpp"
using namespace bnf;

class ConceptHasNotDefinitionException : public SemanticException
{
	public:
		explicit ConceptHasNotDefinitionException() : SemanticException("Concept has not definition in semantic file. Concept name must be follow by" + EQRULE + " or " + ORRULE + " then a rule.",
																		"ConceptNotDefinedException") {};
};

#endif