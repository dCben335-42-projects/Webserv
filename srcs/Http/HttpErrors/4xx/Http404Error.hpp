#ifndef HTTP_404_ERROR_HPP
# define HTTP_404_ERROR_HPP

#include "AHttpError.hpp"

class Http404Error : public AHttpError
{
    public:
        Http404Error() : AHttpError("404 Not Found", 404) {}
};



#endif