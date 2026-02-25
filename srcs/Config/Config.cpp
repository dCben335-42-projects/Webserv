#include "Config.hpp"
#include "Location.hpp"
#include "CgiParams.hpp"
#include "webserv.hpp"

Config::Config() :
	m_ip(""),
	m_port(0),
	m_defaultLocation(NULL)
{
	m_bodyTimeout	   = FALLBACK_BODY_TIMEOUT;
	m_headerTimeout	   = FALLBACK_HEADER_TIMEOUT;
	m_keepAliveTimeout = FALLBACK_KEEP_ALIVE_TIMEOUT;
	m_maxBodySize	   = FALLBACK_MAX_BODY_SIZE;
}

Config::Config(
	const std::string			&ip,
	unsigned short				 port,
	Location					*defaultLoc,
	const Map<const Location *> &locs) :
	m_ip(ip),
	m_port(port),
	m_defaultLocation(defaultLoc),
	m_locations(locs)
{
	m_bodyTimeout	   = FALLBACK_BODY_TIMEOUT;
	m_headerTimeout	   = FALLBACK_HEADER_TIMEOUT;
	m_keepAliveTimeout = FALLBACK_KEEP_ALIVE_TIMEOUT;
	m_maxBodySize	   = FALLBACK_MAX_BODY_SIZE;
}

Config::~Config()
{
	for (Map<const Location *>::iterator it = m_locations.begin(); it != m_locations.end(); ++it)
		delete it->second;
	delete m_defaultLocation;
	m_locations.clear();
}


/**
 *
 * @brief Gets the IP address of the virtual server.
 *
 * @return The IP address as a constant reference to a string.
 *
 */
const std::string &Config::getIp() const
{
	return m_ip;
}
/**
 *
 * @brief Gets the port number of the virtual server.
 *
 * @return The port number as an unsigned short.
 *
 */
unsigned short Config::getPort() const
{
	return m_port;
}


/**
 *
 * @brief Adds a location configuration to the virtual server.
 *
 * @param path The path of the location.
 * @param location The Location object containing the configuration for the specified path.
 *
 */
void Config::setLocation(const std::string &path, const Location &location)
{
	Location *loc = new Location(location);
	m_locations.set(path, loc);
}


/**
 *
 * @brief Sets the IP address of the virtual server.
 *
 * @param ip The IP address to be set.
 *
 */
void Config::setIp(const std::string &ip)
{
	m_ip = ip;
}
/**
 *
 * @brief Sets the port number of the virtual server.
 *
 * @param port The port number to be set.
 *
 */
void Config::setPort(unsigned short port)
{
	m_port = port;
}

/**
 *
 * @brief Sets the keep-alive timeout for the virtual server.
 *
 * @param timeout The keep-alive timeout to be set (in seconds).
 *
 */
void Config::setKeepAliveTimeout(const time_t timeout)
{
	m_keepAliveTimeout = timeout;
}

/**
 *
 * @brief Sets the header timeout for the virtual server.
 *
 * @param timeout The header timeout to be set (in seconds).
 *
 */
void Config::setHeaderTimeout(const time_t timeout)
{
	m_headerTimeout = timeout;
}

/**
 *
 * @brief Sets the body timeout for the virtual server.
 *
 * @param timeout The body timeout to be set (in seconds).
 *
 */
void Config::setBodyTimeout(const time_t timeout)
{
	m_bodyTimeout = timeout;
}

/**
 *
 * @brief Sets the maximum body size for the virtual server.
 *
 * @param maxBodySize The maximum body size to be set (in bytes).
 *
 */
void Config::setMaxBodySize(const size_t maxBodySize)
{
	m_maxBodySize = maxBodySize;
}


/**
 *
 * @brief Gets the index file for the specified location path.
 *
 * @param locationPath The path of the location.
 *
 * @return The index file as a constant reference to a string.
 *
 */
const Vec<std::string> &Config::getIndexFiles(const std::string &locationPath) const
{
	if (m_locations.has(locationPath))
		return m_locations.get(locationPath)->getIndexFiles();
	return m_defaultLocation->getIndexFiles();
}


/**
 *
 * @brief Checks if the specified location path has an index file configured.
 *
 * @param locationPath The path of the location to check.
 *
 * @return true if the location has an index file configured, false otherwise.
 *
 */
bool Config::hasIndexFiles(const std::string &locationPath) const
{
	if (m_locations.has(locationPath))
		return m_locations.get(locationPath)->hasIndexFile();
	if (locationPath == "/")
		return m_defaultLocation->hasIndexFile();
	return false;
}


/**
 *
 * @brief Gets the keep-alive timeout for the virtual server.
 *
 * @return The keep-alive timeout in seconds.
 *
 */
time_t Config::getKeepAliveTimeout() const
{
	return m_keepAliveTimeout;
}

/**
 *
 * @brief Gets the header timeout for the virtual server.
 *
 * @return The header timeout in seconds.
 *
 */
time_t Config::getHeaderTimeout() const
{
	return m_headerTimeout;
}

/**
 *
 * @brief Gets the body timeout for the virtual server.
 *
 * @return The body timeout in seconds.
 *
 */
time_t Config::getBodyTimeout() const
{
	return m_bodyTimeout;
}


/**
 *
 * @brief Gets the maximum body size for the virtual server.
 *
 * @return The maximum body size in bytes.
 *
 */
size_t Config::getMaxBodySize(const std::string &locationPath) const
{
	(void) locationPath;
	return m_maxBodySize;
}


/**
 *
 * @brief Sets the server name for the virtual server.
 *
 * @param serverName The server name to be set.
 *
 */
void Config::setServerNames(const Vec<std::string> &serverNames)
{
	m_serverNames = serverNames;
}


/**
 *
 * @brief Checks if the virtual server has a server name configured.
 *
 * @return true if the server name is configured, false otherwise.
 *
 */
bool Config::hasServerName() const
{
	return m_serverNames.size() > 0;
}


/**
 *
 * @brief Gets the server name for the virtual server.
 *
 * @return The server name as a constant reference to a string.
 *
 */
const Vec<std::string> &Config::getServerNames() const
{
	return m_serverNames;
}

/**
 *
 * @brief Gets the default Location object for the virtual server.
 *
 * @return A pointer to the default Location object.
 *
 */
Location const *Config::getDefaultLocation() const
{
	return m_defaultLocation;
}

/**
 *
 * @brief Gets the map of location paths to their corresponding Location objects.
 *
 * @return A constant reference to the map of locations.
 *
 */
const Map<const Location *> &Config::getLocations() const
{
	return m_locations;
}





#include "LoggerJson.hpp"

std::ostream &operator<<(std::ostream &os, const Config &config)
{
	os << "Config(ip=" << config.getIp()
	   << ", port=" << config.getPort()
	   << ", serverName=" << LoggerJson::get(config.getServerNames())
	   << ", defaultLocation=" << LoggerJson::get(config.getDefaultLocation())
	   << ")";
	return os;
}
