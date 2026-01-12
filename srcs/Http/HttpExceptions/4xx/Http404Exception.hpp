#ifndef HTTP_404_ERROR_HPP
#define HTTP_404_ERROR_HPP

#include "AHttpException.hpp"

class Http404Exception : public AHttpException
{
	public:
		Http404Exception() : AHttpException("404 Not Found", 404)
		{
		}
};



#endif