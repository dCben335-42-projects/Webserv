#ifndef HTTP_403_ERROR_HPP
#define HTTP_403_ERROR_HPP

#include "AHttpException.hpp"

class Http403Exception : public AHttpException
{
	public:
		Http403Exception() : AHttpException("403 Forbidden", 403)
		{
		}
};

#endif
