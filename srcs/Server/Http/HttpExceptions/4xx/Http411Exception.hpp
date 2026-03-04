#ifndef HTTP_411_EXCEPTION_HPP
#define HTTP_411_EXCEPTION_HPP

#include "AHttpException.hpp"

#define HTTP_411_MSG "Length Required"

class Http411Exception : public AHttpException
{
	public:
		Http411Exception() : AHttpException(HTTP_411_MSG, 411)
		{
		}
		Http411Exception(const std::string &extraMsg) : AHttpException(HTTP_411_MSG, 411)
		{
			m_extraMessage = extraMsg;
		}
};

#endif
