#ifndef HTTP_400_ERROR_HPP
#define HTTP_400_ERROR_HPP

#include "AHttpException.hpp"

class Http400Exception : public AHttpException
{
	public:
		Http400Exception() : AHttpException("400 Bad Request", 400)
		{
		}
};

#endif
