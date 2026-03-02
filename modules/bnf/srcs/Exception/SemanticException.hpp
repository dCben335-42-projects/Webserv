#ifndef BNF_SEMANTIC_EXCEPTION_HPP
#define BNF_SEMANTIC_EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include <iostream>

class SemanticException : public std::runtime_error
{
	protected:
		std::string m_classId;

	public:
		SemanticException(const std::string &message, const std::string &classId);
		~SemanticException() throw();

		std::string getClassId() const;
};

std::ostream &operator<<(std::ostream &os, const SemanticException &data);

#endif