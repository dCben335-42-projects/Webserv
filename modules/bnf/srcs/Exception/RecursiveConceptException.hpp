#ifndef RECURSIVE_CONCEPT_EXCEPTION_HPP
#define RECURSIVE_CONCEPT_EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include "SemanticException.hpp"
#include "bnf_defs.hpp"
using namespace bnf;

class RecursiveConceptException : public SemanticException
{
	public:
		explicit RecursiveConceptException(const std::string &concept) : SemanticException("Recursive concept detected: " + concept,
																						   "RecursiveConceptException") {};
};

#endif
