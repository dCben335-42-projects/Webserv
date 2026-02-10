#ifndef VIRTUALSERVER_CONFIG_HPP
#define VIRTUALSERVER_CONFIG_HPP

#include "Vec.hpp"
#include "Map.hpp"

class Config
{
	public:
		class Location;
		class CgiParams;

	private:
		std::string	   m_ip;
		unsigned short m_port;
		std::string	   m_domain;

		Location			 *m_defaultLocation;
		Map<const Location *> m_locations;

		time_t m_keepAliveTimeout;
		time_t m_headerTimeout;
		time_t m_bodyTimeout;

	public:
		Config(const std::string		   &ip,
			   unsigned short				port,
			   const std::string		   &domain,
			   Location					   *defaultLoc,
			   const Map<const Location *> &locs);

		~Config();


		void setIp(const std::string &ip);
		void setPort(unsigned short port);
		void setDomain(const std::string &domain);
		void setKeepAliveTimeout(const time_t timeout);

		const std::string			&getIp() const;
		unsigned short				 getPort() const;
		const std::string			&getDomain() const;
		const Location				*getDefaultLocation() const;
		const Map<const Location *> &getLocations() const;

		const time_t &getKeepAliveTimeout() const;
		const time_t &getHeaderTimeout() const;
		const time_t &getBodyTimeout() const;

		bool			  hasIndexFile(const std::string &locationPath) const;
		const std::string getIndexFile(const std::string &locationPath) const;
};


std::ostream &operator<<(std::ostream &os, const Config &config);

#endif
