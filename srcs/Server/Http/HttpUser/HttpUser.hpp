#ifndef HTTPUSER_HPP
#define HTTPUSER_HPP

#include <string>
#include <ctime>

#include "Map.hpp"
#include <stdint.h>

class HttpCookie
{
	private:
		std::string	   m_uname;
		std::string	   m_pswd;
		std::string	   m_serverName;
		unsigned short m_serverPort;
		time_t		   m_creationDate;
		time_t		   m_expirationDate;
		std::string	   m_stringifiedCookie;

		std::string m_hashStr;
		uint64_t	m_hashInt;
		std::string m_salt;

	public:
		HttpCookie(const std::string name, const std::string pswd, const std::string serverName, unsigned short serverPort);
		HttpCookie();
		~HttpCookie();

		void			   parse(std::string rawCookie);
		void			   stringifyCookie();
		const std::string &getStringifiedCookie() const;
		const std::string &getName() const;
		const std::string &getPswd() const;
		const std::string &getSalt() const;
		const time_t	  &getExpDate() const;

		void	 generateEncryptedPassword();
		uint64_t computeHash(const std::string str) const;

		bool operator==(const HttpCookie &obj) const;
};


#endif