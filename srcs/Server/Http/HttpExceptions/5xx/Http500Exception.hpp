#ifndef HTTP_500_EXCEPTION_HPP
#define HTTP_500_EXCEPTION_HPP

#include "AHttpException.hpp"

#define HTTP_500_ERROR_MSG "Internal Server Error"

class Http500Exception : public AHttpException
{
	public:
		Http500Exception() : AHttpException(HTTP_500_ERROR_MSG, 500)
		{
		}
};

#endif
