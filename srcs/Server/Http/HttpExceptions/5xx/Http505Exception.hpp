#ifndef HTTP_505_ERROR_HPP
#define HTTP_505_ERROR_HPP

#include "AHttpException.hpp"

#define HTTP_505_MSG "HTTP Version Not Supported"

class Http505Exception : public AHttpException
{
	public:
		Http505Exception() : AHttpException(HTTP_505_MSG, 505)
		{
		}
		Http505Exception(const std::string &message) : AHttpException(HTTP_505_MSG, 505)
		{
			m_extraMessage = message;
		}
};

#endif
