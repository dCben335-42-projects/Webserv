#ifndef HTTP_501_ERROR_HPP
#define HTTP_501_ERROR_HPP

#include "AHttpException.hpp"

#define HTTP_501_MSG "Not Implemented"

class Http501Exception : public AHttpException
{
	public:
		Http501Exception(const std::string &method) :
			AHttpException((!method.empty() ? (method + " ") : "") + HTTP_501_MSG, 501)
		{
		}
};

#endif
