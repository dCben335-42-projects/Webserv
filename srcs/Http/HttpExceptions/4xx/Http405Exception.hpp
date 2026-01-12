#ifndef HTTP_405_ERROR_HPP
#define HTTP_405_ERROR_HPP

#include "AHttpException.hpp"

class Http405Exception : public AHttpException
{
	public:
		Http405Exception() : AHttpException("405 Method Not Allowed", 405)
		{
		}
};

#endif
