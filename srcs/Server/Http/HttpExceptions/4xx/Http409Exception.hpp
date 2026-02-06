#ifndef HTTP_409_EXCEPTION_HPP
#define HTTP_409_EXCEPTION_HPP

#include "AHttpException.hpp"

#define HTTP_409_MSG "Conflict"

class Http409Exception : public AHttpException
{
	public:
		Http409Exception() : AHttpException(HTTP_409_MSG, 409)
		{
		}
};

#endif
