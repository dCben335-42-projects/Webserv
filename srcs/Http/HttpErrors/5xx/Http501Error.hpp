#ifndef HTTP_501_ERROR_HPP
#define HTTP_501_ERROR_HPP

#include "AHttpError.hpp"

class Http501Error : public AHttpError
{
	public:
		Http501Error(const std::string &method) :
			AHttpError("501 " + (!method.empty() ? (method + " ") : "") + "Not Implemented", 501)
		{
		}
};

#endif
