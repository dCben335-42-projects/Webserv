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

		std::pair<bool, std::string> m_serverName;

		Location			 *m_defaultLocation;
		Map<const Location *> m_locations;

		time_t m_keepAliveTimeout;
		time_t m_headerTimeout;
		time_t m_bodyTimeout;

		size_t m_maxBodySize;
		size_t m_maxHeadersSize;

	public:
		Config(const std::string		   &ip,
			   unsigned short				port,
			   const std::string		   &serverName,
			   Location					   *defaultLoc,
			   const Map<const Location *> &locs);

		~Config();


		void setIp(const std::string &ip);
		void setPort(unsigned short port);
		void setKeepAliveTimeout(const time_t timeout);
		void setHeaderTimeout(const time_t timeout);
		void setBodyTimeout(const time_t timeout);
		void setMaxBodySize(const size_t maxBodySize);
		void setMaxHeadersSize(const size_t maxHeadersSize);
		void setServerName(const std::string &serverName);

		const std::string			&getIp() const;
		unsigned short				 getPort() const;
		const Location				*getDefaultLocation() const;
		const Map<const Location *> &getLocations() const;

		bool			   hasServerName() const;
		const std::string &getServerName() const;

		time_t getKeepAliveTimeout() const;
		time_t getHeaderTimeout() const;
		time_t getBodyTimeout() const;

		size_t getMaxBodySize() const;
		size_t getMaxHeadersSize() const;

		bool					hasIndexFiles(const std::string &locationPath) const;
		const Vec<std::string> &getIndexFiles(const std::string &locationPath) const;
};


std::ostream &operator<<(std::ostream &os, const Config &config);

#endif
