#ifndef CONCEPT_ALREADY_DEFINED_EXCEPTION_HPP
#define CONCEPT_ALREADY_DEFINED_EXCEPTION_HPP

#include <string>
#include <stdexcept>
#include "SemanticException.hpp"

class ConceptAlreadyDefinedException : public SemanticException
{
    public:
        explicit ConceptAlreadyDefinedException(const std::string &message)
            : SemanticException("Concept \"" + message + "\" is already defined in semantic file.",
                "ConceptAlreadyDefinedException")
        {};
};

#endif