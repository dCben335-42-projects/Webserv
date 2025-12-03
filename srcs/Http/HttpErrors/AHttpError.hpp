#ifndef A_HTTP_ERROR_HPP
#define A_HTTP_ERROR_HPP

#include <string>
#include <stdexcept>

class AHttpError : public std::runtime_error
{
    protected:
        int m_statusCode;
    public:
        explicit AHttpError(const std::string &message, int code): 
            std::runtime_error(message),
            m_statusCode(code)
        {}
        
        int getStatusCode() const {
            return m_statusCode;
        }
};

#endif