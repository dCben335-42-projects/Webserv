#ifndef HTTP_403_ERROR_HPP
#define HTTP_403_ERROR_HPP

#include "AHttpException.hpp"

#define HTTP_403_ERROR_MSG "Forbidden"

class Http403Exception : public AHttpException
{
	public:
		Http403Exception() : AHttpException(HTTP_403_ERROR_MSG, 403)
		{
		}
};

#endif
