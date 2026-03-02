#include "SemanticException.hpp"

SemanticException::SemanticException(const std::string &message, const std::string &classId) : std::runtime_error(message), m_classId(classId)
{
}

SemanticException::~SemanticException() throw()
{
}

std::string SemanticException::getClassId() const
{
	return m_classId;
}

std::ostream &operator<<(std::ostream &os, const SemanticException &data)
{
	std::cout << "[" << data.getClassId() << "]: \033[33m" << data.what() << "\033[0m" << std::endl;
	return (os);
}