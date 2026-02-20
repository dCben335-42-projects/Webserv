#ifndef HTTPAUTH_HPP
#define HTTPAUTH_HPP

#include "Map.hpp"

#define AUTH_REGISTER_NAME_KEY "r_username"
#define AUTH_REGISTER_PSWD_KEY "r_password"
#define AUTH_REGISTER_CONFIRM_KEY "r_confirm_password"

#define AUTH_LOGIN_NAME_KEY "l_username"
#define AUTH_LOGIN_PSWD_KEY "l_password"

#define AUTH_TEMPLATE_AUTHLOC "{{ AUTH_LOC }}"

#define AUTH_REGISTER_LOC "/register"
#define AUTH_LOGIN_LOC "/login"

class HttpAuth
{
	public:
		enum AuthMode
		{
			REGISTER,
			LOGIN
		};

	private:
		Map<std::string>		 m_urlEncoded;
		static const std::string m_templatePage;
		std::string				 m_page;
		const std::string		 m_authLoc;

		void checkField(const std::string field);

	public:
		HttpAuth(const std::string authLoc);
		~HttpAuth();

		void					parse(const std::string &body, AuthMode mode);
		const Map<std::string> &getUrlEncoded() const;

		const std::string &getPage() const;
		void			   generatePage();
};

#endif