#ifndef HTTPUSER_HPP
#define HTTPUSER_HPP

#include <string>

#include "Map.hpp"
#include "AuthToken.hpp"

class HttpUser
{
	private:
		std::string m_uname;
		std::string m_pswd;
		std::string m_salt;
		time_t		m_creationDate;

		unsigned short m_serverPort;

		std::string m_stringifiedCookie;

		AuthToken m_token;

	public:
		HttpUser(const std::string name, const std::string pswd, unsigned short serverPort);
		HttpUser();
		~HttpUser();

		void parse(std::string rawCookie);
		void stringifyCookie();

		bool hasValidToken() const;

		const std::string &getStringifiedCookie() const;
		const std::string &getName() const;
		const std::string &getPswd() const;
		const std::string &getSalt() const;
		const AuthToken	  &getToken() const;

		void refreshToken();

		std::string generateEncryptedPassword(const std::string &pswd);
};

#endif