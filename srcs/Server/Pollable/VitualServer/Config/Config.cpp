#include "Config.hpp"
#include "Location.hpp"

Config::Config() :
	m_ip(""),
	m_port(0),
	m_domain(""),
	defaultLocation(NULL),
	locations()
{
}

Config::Config(
	const std::string	  &ip,
	unsigned short		   port,
	const std::string	  &domain,
	Location			  *defaultLoc,
	const Map<Location *> &locs) :
	m_ip(ip),
	m_port(port),
	m_domain(domain),
	defaultLocation(defaultLoc),
	locations(locs)
{
}

Config::~Config()
{
	delete defaultLocation;
	for (Map<Location *>::iterator it = locations.begin(); it != locations.end(); ++it)
	{
		delete it->second;
	}
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
	if (locations.has(locationPath))
	{
		const std::string &root = locations.get(locationPath)->getRoot();
		if (!root.empty())
			return root;
	}
	return defaultLocation->getRoot();
}
/**
 *
 * @brief Gets the CGI binary directory for the specified location path.
 * @param locationPath The path of the location.
 *
 * @return The CGI binary directory as a constant reference to a string.
 *
 */
const std::string &Config::getCgiBin(const std::string &locationPath) const
{
	(void) locationPath;
	static const std::string emptyString = "";
	return emptyString;
	// CgiParams *cgiParams;
	// if (locations.has(locationPath))
	// {
	// 	CgiParams *cgiParams = locations.get(locationPath);
	// }
	// cgiParams = defaultLocation->m_cgi;
	// if (cgiParams)
	// {
	// 	return cgiParams->getCgiBin();
	// }
	// static const std::string emptyString = "";
	// return emptyString;
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
	if (locations.has(locationPath))
	{
		const std::string &indexFile = locations.get(locationPath)->getIndexFile();
		if (!indexFile.empty())
			return indexFile;
	}
	return defaultLocation->getIndexFile();
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
	if (locations.has(locationPath))
	{
		if (locations.get(locationPath)->getAutoIndex())
			return true;
	}
	return defaultLocation->getAutoIndex();
}
