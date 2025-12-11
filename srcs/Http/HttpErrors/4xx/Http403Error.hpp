#ifndef HTTP_403_ERROR_HPP
#define HTTP_403_ERROR_HPP

#include "AHttpError.hpp"

class Http403Error : public AHttpError
{
	public:
		Http403Error() : AHttpError("403 Forbidden", 403)
		{
		}
};

#endif
