#ifndef HTTP_405_ERROR_HPP
#define HTTP_405_ERROR_HPP

#include "AHttpException.hpp"

class Http406Exception : public AHttpException
{
	public:
		Http406Exception() : AHttpException("406 Not Acceptable", 406)
		{
		}
};

#endif
