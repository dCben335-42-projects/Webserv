#ifndef HTTP_405_ERROR_HPP
#define HTTP_405_ERROR_HPP

#include "AHttpError.hpp"

class Http405Error : public AHttpError
{
	public:
		Http405Error() : AHttpError("4056 Method Not Allowed", 405)
		{
		}
};

#endif
