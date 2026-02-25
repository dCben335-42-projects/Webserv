#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include "Vec.hpp"
#include "Map.hpp"
#include "Location.hpp"


class Config
{
	private:
		std::string	   m_ip;
		unsigned short m_port;

		Vec<std::string> m_serverNames;

		Location			 *m_defaultLocation;
		Map<const Location *> m_locations;

		time_t m_keepAliveTimeout;
		time_t m_headerTimeout;
		time_t m_bodyTimeout;

		size_t m_maxBodySize;

	public:
		Config();
		Config(const std::string		   &ip,
			   unsigned short				port,
			   Location					   *defaultLoc,
			   const Map<const Location *> &locs);
		~Config();

		void setLocation(const std::string &path, const Location &location);

		void setIp(const std::string &ip);
		void setPort(unsigned short port);
		void setKeepAliveTimeout(const time_t timeout);
		void setHeaderTimeout(const time_t timeout);
		void setBodyTimeout(const time_t timeout);
		void setMaxBodySize(const size_t maxBodySize);
		void setServerNames(const Vec<std::string> &serverNames);

		const std::string			&getIp() const;
		unsigned short				 getPort() const;
		const Location				*getDefaultLocation() const;
		const Map<const Location *> &getLocations() const;

		bool					hasServerName() const;
		const Vec<std::string> &getServerNames() const;

		time_t getKeepAliveTimeout() const;
		time_t getHeaderTimeout() const;
		time_t getBodyTimeout() const;

		size_t getMaxBodySize(const std::string &locationPath) const;

		bool					hasIndexFiles(const std::string &locationPath) const;
		const Vec<std::string> &getIndexFiles(const std::string &locationPath) const;
};


std::ostream &operator<<(std::ostream &os, const Config &config);

#endif
