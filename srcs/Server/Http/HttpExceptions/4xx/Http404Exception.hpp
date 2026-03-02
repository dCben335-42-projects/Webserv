#ifndef HTTP_404_ERROR_HPP
#define HTTP_404_ERROR_HPP

#include "AHttpException.hpp"

#define HTTP_404_MSG "Not Found"

class Http404Exception : public AHttpException
{
	public:
		Http404Exception() : AHttpException(HTTP_404_MSG, 404)
		{
		}
		Http404Exception(const std::string &extraMsg) : AHttpException(HTTP_404_MSG, 404)
		{
			m_extraMessage = extraMsg;
		}
};



#endif