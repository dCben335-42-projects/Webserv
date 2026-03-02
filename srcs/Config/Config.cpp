#include "Config.hpp"
#include "Location.hpp"
#include "CgiParams.hpp"
#include "webserv.hpp"

Config::Config()
{
	m_bodyTimeout	   = FALLBACK_BODY_TIMEOUT;
	m_headerTimeout	   = FALLBACK_HEADER_TIMEOUT;
	m_keepAliveTimeout = FALLBACK_KEEP_ALIVE_TIMEOUT;
	m_maxBodySize	   = FALLBACK_MAX_BODY_SIZE;
}

Config::Config(const Config &other) :
	m_address(other.m_address),
	m_serverNames(other.m_serverNames),
	m_defaultLocation(other.m_defaultLocation),
	m_locations(other.m_locations),
	m_keepAliveTimeout(other.m_keepAliveTimeout),
	m_headerTimeout(other.m_headerTimeout),
	m_bodyTimeout(other.m_bodyTimeout),
	m_maxBodySize(other.m_maxBodySize)
{
}


Config::~Config()
{
	m_locations.clear();
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
	m_locations.set(path, location);
}

/**
 *
 * @brief Sets the default location configuration for the virtual server.
 *
 * @param location The Location object containing the default configuration.
 *
 */
void Config::setDefaultLocation(const Location &location)
{
	m_defaultLocation = location;
}


/**
 *
 * @brief Sets the socket address for the virtual server.
 *
 * @param address The SocketAddress object containing the IP and port information.
 *
 */
void Config::setAddress(const SocketAddress &address)
{
	m_address = address;
}

/**
 *
 * @brief Sets the keep-alive timeout for the virtual server.
 *
 * @param timeout The keep-alive timeout to be set (in seconds).
 *
 */
void Config::setKeepAliveTimeout(const size_t timeout)
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
void Config::setHeaderTimeout(const size_t timeout)
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
void Config::setBodyTimeout(const size_t timeout)
{
	m_bodyTimeout = timeout;
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
		return m_locations.get(locationPath).getIndexes();
	return m_defaultLocation.getIndexes();
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
bool Config::hasIndexes(const std::string &locationPath) const
{
	if (m_locations.has(locationPath))
		return m_locations.get(locationPath).hasIndexes();
	if (locationPath == "/")
		return m_defaultLocation.hasIndexes();
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
 * @brief Sets the authentication location for the virtual server.
 *
 * @param authLocation The authentication location to be set.
 *
 */
void Config::setAuthLocation(const std::string &authLocation)
{
	m_authLocation.first  = true;
	m_authLocation.second = authLocation;
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
 * @brief Gets the IP address of the virtual server.
 *
 * @return The IP address as a constant reference to a string.
 *
 */
const SocketAddress &Config::getAddress() const
{
	return m_address;
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
const Location &Config::getDefaultLocation() const
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
const Config::LocationsMap &Config::getLocations() const
{
	return m_locations;
}





#include "LoggerJson.hpp"

std::ostream &operator<<(std::ostream &os, const Config &config)
{
	const Config::LocationsMap &locations = config.getLocations();

	os << "Config(address=" << config.getAddress()
	   << ", serverName=" << LoggerJson::get(config.getServerNames())
	   << ", defaultLocation=" << LoggerJson::get(config.getDefaultLocation())
	   << ", locations size=" << locations.size()
	   << ", Locations=" << LoggerJson::get(locations)
	   << ")";
	return os;
}
