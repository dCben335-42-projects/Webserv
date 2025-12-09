#include "SemanticException.hpp"

std::ostream& operator<<(std::ostream& os, const SemanticException& data)
{
    std::cout << "[" << data.getClassId() << "]: \033[33m" << data.what() << "\033[0m" << std::endl;
    return (os);
}