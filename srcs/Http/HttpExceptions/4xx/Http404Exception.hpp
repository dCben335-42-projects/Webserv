#ifndef HTTP_404_ERROR_HPP
#define HTTP_404_ERROR_HPP

#include "AHttpException.hpp"

#define HTTP_404_ERROR_MSG "Not Found"

class Http404Exception : public AHttpException
{
	public:
		Http404Exception() : AHttpException(HTTP_404_ERROR_MSG, 404)
		{
		}
};



#endif