#ifndef APP_ERRORS_HPP
#define APP_ERRORS_HPP

#include <stdexcept>
#include <string>
#include <sstream>

class AppError : public std::exception
{
	private:
		std::string m_message;
		std::string m_type;
		int			m_code;
		std::string m_file;
		int			m_line;

		mutable std::string m_fullMessage;

	public:
		AppError(
			const std::string &type,
			const std::string &message,
			int				   code = 0,
			const std::string &file = __FILE__,
			int				   line = __LINE__);

		virtual ~AppError() throw();
		virtual const char *what() const throw();

		const std::string &getMessage() const;
		const std::string &getType() const;
		int				   getCode() const;
		const std::string &getFile() const;
		int				   getLine() const;
};

#endif