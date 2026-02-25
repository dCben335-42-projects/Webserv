#ifndef BNF_CACHE_FULL_HPP
#define BNF_CACHE_FULL_HPP

#include <string>
#include <stdexcept>
#include "../SemanticException.hpp"

class CacheFullException : public SemanticException
{
	public:
		explicit CacheFullException() :
			SemanticException("ARulePathInfo cache error due to reaching maximum capacity", "BNF Parsing - CacheFullException") {};
};

#endif