#ifndef HTTP_501_ERROR_HPP
#define HTTP_501_ERROR_HPP

#include "AHttpException.hpp"

class Http501Exception : public AHttpException
{
	public:
		Http501Exception(const std::string &method) :
			AHttpException("501 " + (!method.empty() ? (method + " ") : "") + "Not Implemented", 501)
		{
		}
};

#endif
