#ifndef HTTP_413_EXCEPTION_HPP
#define HTTP_413_EXCEPTION_HPP

#include "AHttpException.hpp"

#define HTTP_413_MSG "Payload Too Large"

class Http413Exception : public AHttpException
{
	public:
		Http413Exception() : AHttpException(HTTP_413_MSG, 413)
		{
		}
};

#endif
