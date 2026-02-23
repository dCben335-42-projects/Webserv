#ifndef HTTP_403_ERROR_HPP
#define HTTP_403_ERROR_HPP

#include "AHttpException.hpp"

#define HTTP_403_MSG "Forbidden"

class Http403Exception : public AHttpException
{
	public:
		Http403Exception() : AHttpException(HTTP_403_MSG, 403)
		{
		}
		Http403Exception(const std::string &extraMsg) : AHttpException(HTTP_403_MSG, 403)
		{
			m_extraMessage = extraMsg;
		}
};

#endif
