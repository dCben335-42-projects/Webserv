#ifndef HTTP_405_ERROR_HPP
#define HTTP_405_ERROR_HPP

#include "AHttpException.hpp"

#define HTTP_405_ERROR_MSG "Method Not Allowed"

class Http405Exception : public AHttpException
{
	public:
		Http405Exception() : AHttpException(HTTP_405_ERROR_MSG, 405)
		{
		}
};

#endif
