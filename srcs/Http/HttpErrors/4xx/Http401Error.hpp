#ifndef HTTP_401_ERROR_HPP
# define HTTP_401_ERROR_HPP

#include "AHttpError.hpp"

class Http401Error: public AHttpError
{
    public:
        Http401Error() : AHttpError("401 Unauthorized", 401) {}
};

#endif
