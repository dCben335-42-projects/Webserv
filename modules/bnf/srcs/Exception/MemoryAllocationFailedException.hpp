#ifndef BNF_MEMORY_ALLOCATION_FAILED_EXCEPTION_HPP
#define BNF_MEMORY_ALLOCATION_FAILED_EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include "SemanticException.hpp"
#include "bnf_defs.hpp"
using namespace bnf;

class MemoryAllocationFailedException : public SemanticException
{
	public:
		explicit MemoryAllocationFailedException() : SemanticException("Memory allocation failed",
																	   "MemoryAllocationFailedException") {};
};

#endif
