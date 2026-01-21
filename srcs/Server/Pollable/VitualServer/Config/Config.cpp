#include "Config.hpp"
#include "Location.hpp"
#include "Logger.hpp"
#include "CgiParams.hpp"

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
}

Config::~Config()
{
	Logger::log("[Config] Destructor called");
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
 * @brief Gets the root directory for the specified location path.
 *
 * @param locationPath The path of the location.
 *
 * @return The root directory as a constant reference to a string.
 *
 */
const std::string &Config::getRoot(const std::string &locationPath) const
{
	if (m_locations.has(locationPath))
	{
		const std::string &root = m_locations.get(locationPath)->getRoot();
		if (!root.empty())
			return root;
	}
	return m_defaultLocation->getRoot();
}
/**
 *
 * @brief Gets the CGI binary directory for the specified location path.
 * @param locationPath The path of the location.
 *
 * @return The CGI binary directory as a constant reference to a string.
 *
 */
const std::string &Config::getCgiBin(const std::string &locationPath, std::string &fileExt) const
{
	(void) locationPath;
	(void) fileExt;
	static const std::string emptyString = "";
	// const Config::CgiParams *cgiParams	 = NULL;

	// if (m_locations.has(locationPath))
	// 	cgiParams = m_locations.get(locationPath)->getCgiParams();
	// if (!cgiParams)
	// 	cgiParams = m_defaultLocation->getCgiParams();
	// if (cgiParams)
	// 	return cgiParams->getBinFromMap(fileExt);
	return emptyString;
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
const std::string &Config::getIndexFile(const std::string &locationPath) const
{
	if (m_locations.has(locationPath))
	{
		const std::string &indexFile = m_locations.get(locationPath)->getIndexFile();
		if (!indexFile.empty())
			return indexFile;
	}
	return m_defaultLocation->getIndexFile();
}

/**
 *
 * @brief Gets whether auto-indexing is enabled for the specified location path.
 *
 * @param locationPath The path of the location.
 *
 * @return true if auto-indexing is enabled, false otherwise.
 *
 */
bool Config::getAutoIndex(const std::string &locationPath) const
{
	if (m_locations.has(locationPath))
	{
		if (m_locations.get(locationPath)->getAutoIndex())
			return true;
	}
	return m_defaultLocation->getAutoIndex();
}

Config::Location const *Config::getDefaultLocation() const
{
	return m_defaultLocation;
}

const Map<const Config::Location *> &Config::getLocations() const
{
	return m_locations;
}





std::ostream &operator<<(std::ostream &os, const Config &config)
{
	os << "Config(ip=" << config.getIp()
	   << ", port=" << config.getPort()
	   << ", domain=" << config.getDomain()
	   << ", defaultLocation=" << LoggerJson::get(config.getDefaultLocation())
	   << ")";
	return os;
}
