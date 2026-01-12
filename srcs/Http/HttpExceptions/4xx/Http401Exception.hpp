#ifndef HTTP_401_ERROR_HPP
#define HTTP_401_ERROR_HPP

#include "AHttpException.hpp"

class Http401Exception : public AHttpException
{
	public:
		Http401Exception() : AHttpException("401 Unauthorized", 401)
		{
		}
};

#endif
