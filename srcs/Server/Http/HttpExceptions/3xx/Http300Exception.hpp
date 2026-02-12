#ifndef HTTP_300_EXCEPTION_HPP
#define HTTP_300_EXCEPTION_HPP

#include "AHttpException.hpp"

#define HTTP_300_MSG "Multiple Choices"

class Http300Exception : public AHttpException
{
	public:
		/**
		 *
		 * @brief Constructs an Http300Exception with a default message.
		 *
		 * @param path The redirection path.
		 *
		 */
		Http300Exception(const std::string &path) : AHttpException(HTTP_300_MSG, 300)
		{
			m_isRedirection	 = true;
			m_redirectionUrl = path;
		}
};

#endif
