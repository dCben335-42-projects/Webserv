#ifndef HTTPCOOKIE_HPP
#define HTTPCOOKIE_HPP

#include <string>
#include <ctime>

#include "Map.hpp"

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

		std::string	 m_hashStr;
		unsigned int m_hashInt;
		unsigned int computeHash(const std::string str);

	public:
		HttpCookie(const std::string name, const std::string pswd, const std::string serverName, unsigned short serverPort);
		HttpCookie();
		~HttpCookie();

		void			   parse(std::string rawCookie);
		void			   stringifyCookie();
		const std::string &getStringifiedCookie() const;
		const std::string &getName() const;
		const std::string &getPswd() const;


		bool operator==(const HttpCookie &obj) const;
};


#endif