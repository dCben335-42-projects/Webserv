#ifndef HTTPCOOKIE_HPP
#define HTTPCOOKIE_HPP

#include <string>
#include <ctime>

class HttpCookie
{
	private:
		std::string m_uname;
		std::string m_pswd;
		std::string m_serverName;
		// unsigned short m_serverPort;
		// time_t		   m_creationDate;
		// time_t		   m_expirationDate;

		std::string m_hash;

	public:
		HttpCookie();
		HttpCookie(const std::string name, const std::string pswd, const std::string serverName);
		void computeHash();
		~HttpCookie();
};


#endif