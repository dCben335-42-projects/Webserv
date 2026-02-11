#ifndef HTTPAUTH_HPP
#define HTTPAUTH_HPP

#include "Map.hpp"

#define AUTH_NAME_KEY "username"
#define AUTH_PSWD_KEY "password"
#define AUTH_CONFIRM_KEY "confirm_password"

class HttpAuth
{
	public:
		enum AuthMode
		{
			REGISTER,
			LOGIN
		};

	private:
		Map<std::string> m_urlEncoded;
		void			 checkField(const std::string field);

	public:
		HttpAuth();
		~HttpAuth();

		void					parse(const std::string &body, AuthMode mode);
		const Map<std::string> &getUrlEncoded() const;
};

#endif