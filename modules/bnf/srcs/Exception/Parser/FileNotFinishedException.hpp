#ifndef BNF_FILE_NOT_FINISHED_EXCEPTION_HPP
#define BNF_FILE_NOT_FINISHED_EXCEPTION_HPP

#include <string>
#include <stdexcept>
#include "../SemanticException.hpp"

class FileNotFinishedException : public SemanticException
{
	public:
		explicit FileNotFinishedException(const std::string &message) :
			SemanticException(message, "BNF Parsing - FileNotFinishedException") {};
};

#endif