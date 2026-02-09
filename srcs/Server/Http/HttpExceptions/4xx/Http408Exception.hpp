#ifndef HTTP_408_ERROR_HPP
#define HTTP_408_ERROR_HPP

#include "AHttpException.hpp"

#define HTTP_408_MSG "Request Timeout"

class Http408Exception : public AHttpException
{
	public:
		Http408Exception() : AHttpException(HTTP_408_MSG, 408)
		{
		}
};

#endif
