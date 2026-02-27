#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include "Vec.hpp"
#include "Map.hpp"
#include "Location.hpp"
#include "SocketAddress.hpp"

class Config;

typedef Vec<Config> ServerConfigs;

typedef Map<SocketAddress, ServerConfigs> VServersConfigs;
typedef VServersConfigs::const_iterator	  VServersConfCIt;

class Config
{
	public:
		typedef Map<Location> LocationsMap;
		typedef Vec<Location> Locations;

	private:
		SocketAddress	 m_address;
		Vec<std::string> m_serverNames;

		Location	 m_defaultLocation;
		LocationsMap m_locations;

		std::pair<bool, std::string> m_authLocation;

		time_t m_keepAliveTimeout;
		time_t m_headerTimeout;
		time_t m_bodyTimeout;
		size_t m_maxBodySize;

	public:
		Config();
		Config(const Config &other);
		~Config();

		void setLocation(const std::string &path, const Location &location);
		void setDefaultLocation(const Location &location);

		void setAddress(const SocketAddress &address);
		void setKeepAliveTimeout(const time_t timeout);
		void setHeaderTimeout(const time_t timeout);
		void setBodyTimeout(const time_t timeout);
		void setMaxBodySize(const size_t maxBodySize);
		void setServerNames(const Vec<std::string> &serverNames);
		void setAuthLocation(const std::string &authLocation);
		void setUploadLocation(const std::string &uploadLocation);

		const SocketAddress &getAddress() const;
		const Location		&getDefaultLocation() const;
		const LocationsMap	&getLocations() const;

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
