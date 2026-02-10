#ifndef HTTP_504_ERROR_HPP
#define HTTP_504_ERROR_HPP

#include "AHttpException.hpp"

#define HTTP_504_MSG "Gateway Timeout"

class Http504Exception : public AHttpException
{
	public:
		Http504Exception() : AHttpException(HTTP_504_MSG, 504)
		{
		}
};

#endif
