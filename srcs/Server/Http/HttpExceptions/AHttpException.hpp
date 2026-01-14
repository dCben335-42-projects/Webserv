#ifndef A_HTTP_ERROR_HPP
#define A_HTTP_ERROR_HPP

#include <string>
#include <stdexcept>
#include "String.hpp"
#include "Number.hpp"

class AHttpException : public std::runtime_error
{
	private:
		static const std::string HEAD_TEMPLATE;
		static const std::string HTML_TEMPLATE;

	protected:
		int m_statusCode;

	public:
		AHttpException(const std::string &message, int code);
		int			getStatusCode() const;
		std::string getPage() const;
};


#endif