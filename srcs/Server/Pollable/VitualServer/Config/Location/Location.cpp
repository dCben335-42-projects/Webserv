#include "Location.hpp"


Config::Location::Location(
	const std::string		 path,
	const std::string		 root,
	const std::string		 alias,
	const std::string		 indexFile,
	const bool				 autoIndex,
	const Config::CgiParams *cgi) :
	m_path(path),
	m_alias(alias),
	m_root(root),
	m_indexFile(indexFile),
	m_autoIndex(autoIndex),
	m_cgi(cgi),
	m_isAutoIndex(true)
{
}

Config::Location::Location(
	const std::string		 path,
	const std::string		 root,
	const std::string		 alias,
	const std::string		 indexFile,
	const bool				 autoIndex,
	const Config::CgiParams *cgi) :
	m_path(path),
	m_alias(alias),
	m_root(root),
	m_indexFile(indexFile),
	m_cgi(cgi),
	m_isAutoIndex(false),
	m_autoIndex(false)
{
}

Config::Location::~Location()
{
}


/**
 *
 * @brief Gets the path of the location.
 *
 * @return The path as a constant reference to a string.
 *
 */
const std::string &Config::Location::getPath() const
{
	return m_path;
}
/**
 *
 * @brief Gets the root directory of the location.
 *
 * @return The root directory as a constant reference to a string.
 *
 */
const std::string &Config::Location::getRoot() const
{
	return m_root;
}
/**
 *
 * @brief Gets the index file of the location.
 *
 * @return The index file as a constant reference to a string.
 *
 */
const std::string &Config::Location::getIndexFile() const
{
	return m_indexFile;
}
/**
 *
 * @brief Gets whether auto-indexing is enabled for the location.
 *
 * @return true if auto-indexing is enabled, false otherwise.
 *
 */
bool Config::Location::getAutoIndex() const
{
	return m_autoIndex;
}
/**
 *
 * @brief Gets the CGI parameters of the location.
 *
 * @return A constant reference to the CGI parameters as a string.
 *
 */
const Config::CgiParams *Config::Location::getCgiParams() const
{
	return m_cgi;
}



/**
 *
 * @brief Checks if the location has an alias set.
 *
 * @return true if an alias is set, false otherwise.
 *
 */
bool Config::Location::hasAlias() const
{
	return !m_alias.empty();
}
/**
 *
 * @brief Checks if the location has CGI parameters set.
 *
 * @return true if CGI parameters are set, false otherwise.
 *
 */
bool Config::Location::hasCgiParams() const
{
	return m_cgi != NULL;
}
/**
 *
 * @brief Checks if the location has an index file set.
 *
 * @return true if an index file is set, false otherwise.
 *
 */
bool Config::Location::hasIndexFile() const
{
	return !m_indexFile.empty();
}
/**
 *
 * @brief Checks if the location has a root directory set.
 *
 * @return true if a root directory is set, false otherwise.
 *
 */
bool Config::Location::hasRoot() const
{
	return !m_root.empty();
}
/**
 *
 * @brief Checks if the auto-index setting has been explicitly set.
 *
 * @return true if the auto-index setting has been set, false otherwise.
 *
 */
bool Config::Location::hasAutoIndexSet() const
{
	return m_isAutoIndex;
}





/**
 *
 * @brief Sets the path of the location.
 *
 * @param path The path to be set.
 *
 */
void Config::Location::setPath(const std::string &path)
{
	m_path = path;
}
/**
 *
 * @brief Sets the root directory of the location.
 *
 * @param root The root directory to be set.
 *
 */
void Config::Location::setRoot(const std::string &root)
{
	m_root = root;
}
/**
 *
 * @brief Sets the index file of the location.
 *
 * @param indexFile The index file to be set.
 *
 */
void Config::Location::setIndexFile(const std::string &indexFile)
{
	m_indexFile = indexFile;
}
/**
 *
 * @brief Sets whether auto-indexing is enabled for the location.
 *
 * @param autoIndex A boolean indicating whether to enable auto-indexing.
 *
 */
void Config::Location::setAutoIndex(const bool autoIndex)
{
	m_autoIndex = autoIndex;
}
/**
 *
 * @brief Sets the CGI parameters for the location.
 *
 * @param cgi A pointer to the CgiParams object to be set.
 *
 *
 */
void Config::Location::setCgiParams(const Config::CgiParams *cgi)
{
	m_cgi = cgi;
}





std::ostream &operator<<(std::ostream &os, const Config::Location &location)
{
	os << "Location(path: " << location.getPath();
	os << ", root: " << location.getRoot();
	os << ", indexFile: " << location.getIndexFile();
	os << ", autoIndex: " << (location.getAutoIndex() ? "true" : "false");
	os << ")";
	return os;
}
