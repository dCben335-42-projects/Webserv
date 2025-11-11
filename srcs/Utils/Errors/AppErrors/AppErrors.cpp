#include "AppErrors.hpp"

AppError::AppError(
    const std::string& type,
    const std::string& message,
    int code,
    const std::string& file,
    int line
) : 
    m_message(message),
    m_type(type),
    m_code(code),
    m_file(file),
    m_line(line)
{
}

AppError::~AppError() throw() {}

const char* AppError::what() const throw()
{
    if (m_fullMessage.empty()) {
        std::ostringstream oss;
        oss << "[" << m_type << "] " << m_message;
        if (m_code)
            oss << " (code: " << m_code << ")";
        if (!m_file.empty())
            oss << " @ " << m_file << ":" << m_line;
        m_fullMessage = oss.str();
    }
    return m_fullMessage.c_str();
}

// ANCHOR Getters
const std::string& AppError::getMessage() const {
    return m_message;
}
const std::string& AppError::getType() const {
    return m_type;
}
int AppError::getCode() const {
    return m_code;
}
const std::string& AppError::getFile() const {
    return m_file;
}
int AppError::getLine() const {
    return m_line;
}

