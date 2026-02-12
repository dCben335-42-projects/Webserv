#ifndef HTTP_301_EXCEPTION_HPP
#define HTTP_301_EXCEPTION_HPP

#include "AHttpException.hpp"

#define HTTP_301_MSG "Moved Permanently"

class Http301Exception : public AHttpException
{
	public:
		/**
		 *
		 * @brief Constructs an Http301Exception with a redirection path.
		 *
		 * @param path The redirection path.
		 *
		 */
		Http301Exception(const std::string path) : AHttpException(HTTP_301_MSG, 301)
		{
			m_isRedirection	 = true;
			m_redirectionUrl = path;
		}
};

#endif
