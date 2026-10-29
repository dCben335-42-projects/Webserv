#ifndef BNF_CONCEPT_NOT_FOUND_EXCEPTION_HPP
#define BNF_CONCEPT_NOT_FOUND_EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include "../SemanticException.hpp"
#include "bnf_defs.hpp"

using namespace bnf;

class ConceptNotFoundException : public SemanticException
{
	public:
		explicit ConceptNotFoundException(const std::string &c) :
			SemanticException(
				"Concept not found: " + c,
				"ConceptNotFoundException") {};
};

#endif