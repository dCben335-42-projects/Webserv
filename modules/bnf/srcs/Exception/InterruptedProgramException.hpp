#ifndef BNF_INTERRUPTED_PROGRAM_EXCEPTION_HPP
#define BNF_INTERRUPTED_PROGRAM_EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include "SemanticException.hpp"

class InterruptedProgramException : public SemanticException
{
	public:
		explicit InterruptedProgramException(const std::string &message) : SemanticException("\"" + message + "\" program was interrupted by a termination signal.",
																							 "InterruptedProgramException") {};
};

#endif
