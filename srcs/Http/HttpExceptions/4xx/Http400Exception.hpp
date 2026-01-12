#ifndef HTTP_400_ERROR_HPP
#define HTTP_400_ERROR_HPP

#include "AHttpException.hpp"

#define HTTP_400_ERROR_MSG "Bad Request"

class Http400Exception : public AHttpException
{
	public:
		Http400Exception() : AHttpException(HTTP_400_ERROR_MSG, 400)
		{
		}
};

#endif
