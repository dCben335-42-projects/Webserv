#ifndef HTTP_400_ERROR_HPP
#define HTTP_400_ERROR_HPP

#include "AHttpException.hpp"

#define HTTP_400_MSG "Bad Request"

class Http400Exception : public AHttpException
{
	public:
		Http400Exception() : AHttpException(HTTP_400_MSG, 400)
		{
		}
		Http400Exception(const std::string &extraMsg) : AHttpException(HTTP_400_MSG, 400)
		{
			m_extraMessage = extraMsg;
		}
};

#endif
