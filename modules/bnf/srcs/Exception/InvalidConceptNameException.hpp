#ifndef INVALID_CONCEPT_NAME_EXCEPTION_HPP
#define INVALID_CONCEPT_NAME_EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include "SemanticException.hpp"

class InvalidConceptNameException : public SemanticException
{
    public:
        explicit InvalidConceptNameException(const std::string &message)
            : SemanticException("\"" + message + "\" is an invalid concept name. It must contain only lowercase or uppercase letters.",
                "InvalidConceptNameException")
        {};
};

#endif
