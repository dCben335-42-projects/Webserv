#ifndef AHTTPERROR_HPP
#define AHTTPERROR_HPP

#include <string>
#include <stdexcept>

class HttpError : public std::runtime_error
{
    protected:
        int statusCode;
    public:
        explicit HttpError(const std::string &message, int code): 
            std::runtime_error(message),
            statusCode(code)
        {}
        
        int getStatusCode() const {
            return statusCode;
        }
};

#endif