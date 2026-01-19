#ifndef VIRTUALSERVER_CONFIG_HPP
#define VIRTUALSERVER_CONFIG_HPP

#include <string>
#include "Vector.hpp"
#include "Map.hpp"
#include "HttpMethod.hpp"

class Config
{
	public:
		class Location;
		class CgiParams;

	private:
		std::string	   m_ip;
		unsigned short m_port;
		std::string	   m_domain;

		Location	   *defaultLocation;
		Map<Location *> locations;

	public:
		Config();
		Config(const std::string	 &ip,
			   unsigned short		  port,
			   const std::string	 &domain,
			   Location				 *defaultLoc,
			   const Map<Location *> &locs);

		~Config();

		const std::string &getIp() const;
		unsigned short	   getPort() const;
		const std::string &getDomain() const;

		void setIp(const std::string &ip);
		void setPort(unsigned short port);
		void setDomain(const std::string &domain);


		const std::string &getRoot(const std::string &locationPath) const;
		const std::string &getCgiBin(const std::string &locationPath, std::string &fileExt) const;
		const std::string &getIndexFile(const std::string &locationPath) const;
		bool			   getAutoIndex(const std::string &locationPath) const;
};


#endif