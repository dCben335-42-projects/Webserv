#ifndef BNF_INVALID_ENTRY_EXCEPTION_HPP
#define BNF_INVALID_ENTRY_EXCEPTION_HPP

#include <string>
#include <stdexcept>
#include "../SemanticException.hpp"

class InvalidEntryException : public SemanticException
{
	public:
		explicit InvalidEntryException(const std::string &message) :
			SemanticException(
				message,
				"BNF Parsing - InvalidEntryException") {};
};

#endif