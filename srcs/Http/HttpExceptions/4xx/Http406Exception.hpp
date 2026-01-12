#ifndef HTTP_405_ERROR_HPP
#define HTTP_405_ERROR_HPP

#include "AHttpException.hpp"

#define HTTP_406_ERROR_MSG "Not Acceptable"

class Http406Exception : public AHttpException
{
	public:
		Http406Exception() : AHttpException(HTTP_406_ERROR_MSG, 406)
		{
		}
};

#endif
