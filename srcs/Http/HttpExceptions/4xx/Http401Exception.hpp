#ifndef HTTP_401_ERROR_HPP
#define HTTP_401_ERROR_HPP

#include "AHttpException.hpp"

#define HTTP_401_ERROR_MSG "Unauthorized"

class Http401Exception : public AHttpException
{
	public:
		Http401Exception() : AHttpException(HTTP_401_ERROR_MSG, 401)
		{
		}
};

#endif
