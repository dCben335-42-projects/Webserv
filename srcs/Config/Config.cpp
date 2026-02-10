#include "Config.hpp"
#include "Location.hpp"
#include "CgiParams.hpp"
#include "webserv.hpp"

Config::Config(
	const std::string			&ip,
	unsigned short				 port,
	const std::string			&domain,
	Location					*defaultLoc,
	const Map<const Location *> &locs) :
	m_ip(ip),
	m_port(port),
	m_domain(domain),
	m_defaultLocation(defaultLoc),
	m_locations(locs)
{
	m_bodyTimeout	   = FALLBACK_BODY_TIMEOUT;
	m_headerTimeout	   = FALLBACK_HEADER_TIMEOUT;
	m_keepAliveTimeout = FALLBACK_KEEP_ALIVE_TIMEOUT;
}

Config::~Config()
{
	for (Map<const Location *>::iterator it = m_locations.begin(); it != m_locations.end(); ++it)
		delete it->second;
	delete m_defaultLocation;
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
 * @brief Gets the domain name of the virtual server.
 *
 * @return The domain name as a constant reference to a string.
 *
 */
const std::string &Config::getDomain() const
{
	return m_domain;
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
 * @brief Sets the domain name of the virtual server.
 *
 * @param domain The domain name to be set.
 *
 */
void Config::setDomain(const std::string &domain)
{
	m_domain = domain;
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
 * @brief Gets the index file for the specified location path.
 *
 * @param locationPath The path of the location.
 *
 * @return The index file as a constant reference to a string.
 *
 */
const std::string Config::getIndexFile(const std::string &locationPath) const
{
	if (m_locations.has(locationPath))
		return m_locations.get(locationPath)->getIndexFile();
	if (locationPath == "/")
		return m_defaultLocation->getIndexFile();
	return "";
}

bool Config::hasIndexFile(const std::string &locationPath) const
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
const time_t &Config::getKeepAliveTimeout() const
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
const time_t &Config::getHeaderTimeout() const
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
const time_t &Config::getBodyTimeout() const
{
	return m_bodyTimeout;
}

/**
 *
 * @brief Gets the default Location object for the virtual server.
 *
 * @return A pointer to the default Location object.
 *
 */
Config::Location const *Config::getDefaultLocation() const
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
const Map<const Config::Location *> &Config::getLocations() const
{
	return m_locations;
}




#include "LoggerJson.hpp"

std::ostream &operator<<(std::ostream &os, const Config &config)
{
	os << "Config(ip=" << config.getIp()
	   << ", port=" << config.getPort()
	   << ", domain=" << config.getDomain()
	   << ", defaultLocation=" << LoggerJson::get(config.getDefaultLocation())
	   << ")";
	return os;
}
