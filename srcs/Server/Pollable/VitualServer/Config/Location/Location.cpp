#include "Location.hpp"

Config::Location::Location() :
	m_path(""),
	m_root(""),
	m_indexFile(""),
	m_autoIndex(false),
	m_cgi(NULL)
{
}

Config::Location::Location(
	std::string		   path,
	std::string		   root,
	std::string		   indexFile,
	bool			   autoIndex,
	Config::CgiParams *cgi) :
	m_path(path),
	m_root(root),
	m_indexFile(indexFile),
	m_autoIndex(autoIndex),
	m_cgi(cgi)
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
const Config::CgiParams &Config::Location::getCgiParams() const
{
	return *m_cgi;
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
void Config::Location::setAutoIndex(bool autoIndex)
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
void Config::Location::setCgiParams(Config::CgiParams *cgi)
{
	m_cgi = cgi;
}
