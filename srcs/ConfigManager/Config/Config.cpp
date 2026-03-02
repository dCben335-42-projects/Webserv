#include "Config.hpp"
#include "Location.hpp"
#include "webserv.hpp"

Config::Config()
{
	m_bodyTimeout	   = FALLBACK_BODY_TIMEOUT;
	m_headerTimeout	   = FALLBACK_HEADER_TIMEOUT;
	m_keepAliveTimeout = FALLBACK_KEEP_ALIVE_TIMEOUT;
	m_accessLog		   = FALLBACK_ACCESS_LOG;
	m_errorLog		   = FALLBACK_ERROR_LOG;
}

Config::Config(const Config &other)
{
	m_address		  = other.m_address;
	m_serverNames	  = other.m_serverNames;
	m_defaultLocation = other.m_defaultLocation;
	m_locations		  = other.m_locations;
	m_authLocation	  = other.m_authLocation;

	m_keepAliveTimeout = other.m_keepAliveTimeout;
	m_headerTimeout	   = other.m_headerTimeout;
	m_bodyTimeout	   = other.m_bodyTimeout;

	m_accessLog = other.m_accessLog;
	m_errorLog	= other.m_errorLog;
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
 * @brief Sets the access log file path for the virtual server.
 *
 * @param accessLog The access log file path to be set.
 *
 */
void Config::setAccessLog(const std::string &accessLog)
{
	m_accessLog = accessLog;
}

/**
 *
 * @brief Sets the error log file path for the virtual server.
 *
 * @param errorLog The error log file path to be set.
 *
 */
void Config::setErrorLog(const std::string &errorLog)
{
	m_errorLog = errorLog;
}


/**
 *
 * @brief Checks if the virtual server has an authentication location configured.
 *
 * @return true if the authentication location is configured, false otherwise.
 *
 */
bool Config::hasAuthLocation() const
{
	return m_authLocation.first;
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
 * @brief Gets the authentication location for the virtual server.
 *
 * @return The authentication location as a constant reference to a string.
 *
 */
const std::string &Config::getAuthLocation() const
{
	return m_authLocation.second;
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
 * @brief Gets the access log file path for the virtual server.
 *
 * @return The access log file path as a constant reference to a string.
 *
 */
const std::string &Config::getAccessLog() const
{
	return m_accessLog;
}

/**
 *
 * @brief Gets the error log file path for the virtual server.
 *
 * @return The error log file path as a constant reference to a string.
 *
 */
const std::string &Config::getErrorLog() const
{
	return m_errorLog;
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


#include "Logger.hpp"

std::ostream &operator<<(std::ostream &os, const Config &config)
{
	const Config::LocationsMap &locations = config.getLocations();

	os << "Config(\n";
	Logger::incrementIndent();
	os << Logger::indent() << "address=" << LoggerJson::get(config.getAddress()) << "\n";
	if (config.hasServerName())
		os << Logger::indent() << "serverNames=" << LoggerJson::get(config.getServerNames()) << "\n";
	if (config.hasAuthLocation())
		os << Logger::indent() << "authLocation=" << LoggerJson::get(config.getAuthLocation()) << "\n";
	os << Logger::indent() << "keepAliveTimeout=" << config.getKeepAliveTimeout() << "\n";
	os << Logger::indent() << "headerTimeout=" << config.getHeaderTimeout() << "\n";
	os << Logger::indent() << "bodyTimeout=" << config.getBodyTimeout() << "\n";
	os << Logger::indent() << "accessLog=" << LoggerJson::get(config.getAccessLog()) << "\n";
	os << Logger::indent() << "errorLog=" << LoggerJson::get(config.getErrorLog()) << "\n";
	os << Logger::indent() << "defaultLocation=" << LoggerJson::get(config.getDefaultLocation()) << "\n";
	if (locations.size() > 0)
		os << Logger::indent() << "Locations=" << LoggerJson::get(locations) << "\n";
	Logger::decrementIndent();
	os << Logger::indent() << ")";
	return os;
}
