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
		int			m_statusCode;
		std::string m_extraMessage;

		bool		m_isRedirection;
		std::string m_redirectionUrl;

	public:
		AHttpException(const std::string &message, int code);
		virtual ~AHttpException() throw();

		int				   getStatusCode() const;
		const std::string  getPage() const;
		const std::string &getExtraMessage() const;

		bool			   isRedirection() const;
		const std::string &getRedirectionUrl() const;
};


#endif