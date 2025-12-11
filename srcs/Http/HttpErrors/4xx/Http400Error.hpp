#ifndef HTTP_400_ERROR_HPP
#define HTTP_400_ERROR_HPP

#include "AHttpError.hpp"

class Http400Error : public AHttpError
{
	public:
		Http400Error() : AHttpError("400 Bad Request", 400)
		{
		}
};

#endif
