#ifndef HTTP404ERROR_HPP
#define HTTP404ERROR_HPP

#include "AHttError.hpp"

class Http404Error : public HttpError
{
    public:
        Http404Error() : HttpError("404 Not Found", 404) {}
};



#endif