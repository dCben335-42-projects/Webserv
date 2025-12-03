#ifndef CONCEPT_ALREADY_DEFINED_EXCEPTION_HPP
#define CONCEPT_ALREADY_DEFINED_EXCEPTION_HPP

#include <string>
#include <stdexcept>

class ConceptAlreadyDefinedException : public std::runtime_error
{
    public:
        explicit ConceptAlreadyDefinedException(const std::string &message):
            std::runtime_error("\033[33mConceptAlreadyDefinedException: Concept " +
                message + " is already defined in semantic file.\033[0m\n"
            )
        {};
};

#endif