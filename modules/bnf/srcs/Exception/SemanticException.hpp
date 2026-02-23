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
		explicit SemanticException(const std::string &message, const std::string &classId) : std::runtime_error(message), m_classId(classId)
		{
		}
		~SemanticException() throw()
		{
		}
		std::string getClassId() const
		{
			return m_classId;
		}
};

std::ostream &operator<<(std::ostream &os, const SemanticException &data);

#endif