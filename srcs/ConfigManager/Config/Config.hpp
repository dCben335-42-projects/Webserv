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
		time_t m_cgiTimeout;

		std::string m_accessLog;
		std::string m_errorLog;

		std::string m_defaultMime;
		std::string m_defaultCgiMime;

	public:
		Config();
		Config(const Config &other);
		~Config();

		void				 setAddress(const SocketAddress &address);
		const SocketAddress &getAddress() const;

		void				setLocation(const std::string &path, const Location &location);
		const LocationsMap &getLocations() const;

		void			setDefaultLocation(const Location &location);
		const Location &getDefaultLocation() const;

		void			   setAuthLocation(const std::string &authLocation);
		bool			   hasAuthLocation() const;
		const std::string &getAuthLocation() const;

		void					setServerNames(const Vec<std::string> &serverNames);
		bool					hasServerName() const;
		const Vec<std::string> &getServerNames() const;

		void   setKeepAliveTimeout(const size_t timeout);
		time_t getKeepAliveTimeout() const;

		void   setHeaderTimeout(const size_t timeout);
		time_t getHeaderTimeout() const;

		void   setBodyTimeout(const size_t timeout);
		time_t getBodyTimeout() const;

		void   setCgiTimeout(const size_t timeout);
		time_t getCgiTimeout() const;

		void			   setAccessLog(const std::string &accessLog);
		const std::string &getAccessLog() const;

		void			   setErrorLog(const std::string &errorLog);
		const std::string &getErrorLog() const;

		void			   setDefaultMime(const std::string &defaultType);
		const std::string &getDefaultMime() const;

		void			   setDefaultCgiMime(const std::string &defaultCgiType);
		const std::string &getDefaultCgiMime() const;

		bool					hasIndexes(const std::string &locationPath) const;
		const Vec<std::string> &getIndexFiles(const std::string &locationPath) const;
};


std::ostream &operator<<(std::ostream &os, const Config &config);

#endif
