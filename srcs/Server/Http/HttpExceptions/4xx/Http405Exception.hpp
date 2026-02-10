#ifndef HTTP_405_ERROR_HPP
#define HTTP_405_ERROR_HPP

#include "AHttpException.hpp"

#define HTTP_405_MSG "Method Not Allowed"

class Http405Exception : public AHttpException
{
	public:
		Http405Exception(const std::string &method) :
			AHttpException((!method.empty() ? (method + " ") : "") + HTTP_405_MSG, 405)
		{
		}
};

#endif
