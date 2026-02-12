#ifndef AUTHTOKEN_HPP
#define AUTHTOKEN_HPP

#include <string>
#include <stdint.h>
#include <ctime>

class AuthToken
{
	private:
		time_t m_creationDate;
		time_t m_expirationDate;

		std::string m_hashStr;
		uint64_t	m_hashInt;

	public:
		AuthToken(const std::string &name, const std::string &serverName, unsigned short serverPort);
		AuthToken(const std::string &hash);
		~AuthToken();

		time_t			   getExpDate() const;
		const std::string &getHashStr() const;
		void			   setHashStr(const std::string &newHash);

		static uint64_t computeHash(const std::string str);

		bool operator==(const AuthToken &obj) const;
};

#endif