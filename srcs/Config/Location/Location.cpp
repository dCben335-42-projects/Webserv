#include "Location.hpp"
#include "Number.hpp"


Location::Location()
{
	m_path = "";
	m_cgi  = NULL;

	m_autoIndex = std::make_pair(false, false);

	m_root	 = std::make_pair(false, std::string());
	m_alias	 = std::make_pair(false, std::string());
	m_upload = std::make_pair(false, Upload());

	m_indexFiles	 = std::make_pair(false, Vec<std::string>());
	m_allowedMethods = std::make_pair(false, Set<HttpMethod>());
}

/**
 *
 * @brief Constructs a Location object with auto-index setting.
 *
 * @param path The path of the location.
 *
 * @note m_isAutoIndex is set to true in this constructor.
 *
 */
Location::Location(const std::string path)
{
	m_path = path;
	m_cgi  = NULL;

	m_autoIndex = std::make_pair(false, false);

	m_root	 = std::make_pair(false, std::string());
	m_alias	 = std::make_pair(false, std::string());
	m_upload = std::make_pair(false, Upload());

	m_indexFiles	 = std::make_pair(false, Vec<std::string>());
	m_allowedMethods = std::make_pair(false, Set<HttpMethod>());
}


Location::Location(const Location &other)
{
	m_path = other.m_path;
	m_cgi  = other.m_cgi ? new CgiParams(*other.m_cgi) : NULL;

	m_root			 = other.m_root;
	m_alias			 = other.m_alias;
	m_upload		 = other.m_upload;
	m_autoIndex		 = other.m_autoIndex;
	m_return		 = other.m_return;
	m_indexFiles	 = other.m_indexFiles;
	m_allowedMethods = other.m_allowedMethods;
}

Location::~Location()
{
	if (m_cgi)
		delete m_cgi;
}


/**
 *
 * @brief Compares two Location objects based on their paths.
 *
 * @param a Pointer to the first Location object.
 * @param b Pointer to the second Location object.
 *
 * @note It is meant to be used for sorting Location objects. (std::sort)
 *
 * @return true if the path of the first Location is less than that of the second, false otherwise.
 *
 */
bool Location::compare(const Location &a, const Location &b)
{
	return a.m_path < b.m_path;
}





/**
 *
 * @brief Gets the path of the location.
 *
 * @return The path as a constant reference to a string.
 *
 */
const std::string &Location::getPath() const
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
const std::string &Location::getRoot() const
{
	return m_root.second;
}
/**
 *
 * @brief Gets the index file of the location.
 *
 * @return The index file as a constant reference to a string.
 *
 */
const Vec<std::string> &Location::getIndexFiles() const
{
	return m_indexFiles.second;
}
/**
 *
 * @brief Gets the alias of the location.
 *
 * @return The alias as a constant reference to a string.
 *
 */
const std::string &Location::getAlias() const
{
	return m_alias.second;
}
/**
 *
 * @brief Gets whether auto-indexing is enabled for the location.
 *
 * @return true if auto-indexing is enabled, false otherwise.
 *
 */
bool Location::getAutoIndex() const
{
	return m_autoIndex.second;
}
/**
 *
 * @brief Gets the CGI parameters of the location.
 *
 * @return A constant reference to the CGI parameters as a string.
 *
 */
const CgiParams *Location::getCgiParams() const
{
	return m_cgi;
}

/**
 *
 * @brief Gets the allowed HTTP methods for the location.
 *
 * @return A constant reference to the set of allowed HTTP methods.
 *
 */
const Set<HttpMethod> &Location::getAllowedMethods() const
{
	return m_allowedMethods.second;
}

/**
 *
 * @brief Gets whether file uploads are allowed for the location.
 *
 * @return true if file uploads are allowed, false otherwise.
 *
 */
const Upload &Location::getUpload() const
{
	return m_upload.second;
}

/**
 *
 * @brief Gets the return value for the location.
 *
 * @return A constant reference to the LocationReturn object representing the return value.
 *
 */
const LocationReturn &Location::getReturn() const
{
	return m_return.second;
}

/**
 *
 * @brief Gets the maximum body size for the location.
 *
 * @return The maximum body size in bytes.
 *
 */
size_t Location::getMaxBodySize() const
{
	return m_maxBodySize.second;
}





/**
 *
 * @brief Checks if the location has an alias set.
 *
 * @return true if an alias is set, false otherwise.
 *
 */
bool Location::hasAlias() const
{
	return m_alias.first;
}
/**
 *
 * @brief Checks if the location has CGI parameters set.
 *
 * @return true if CGI parameters are set, false otherwise.
 *
 */
bool Location::hasCgiParams() const
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
bool Location::hasIndexFile() const
{
	return m_indexFiles.first;
}
/**
 *
 * @brief Checks if the location has a root directory set.
 *
 * @return true if a root directory is set, false otherwise.
 *
 */
bool Location::hasRoot() const
{
	return m_root.first;
}
/**
 *
 * @brief Checks if the auto-index setting has been explicitly set.
 *
 * @return true if the auto-index setting has been set, false otherwise.
 *
 */
bool Location::hasAutoIndex() const
{
	return m_autoIndex.first;
}

/**
 *
 * @brief Checks if the location has allowed HTTP methods set.
 *
 * @return true if allowed methods are set, false otherwise.
 *
 */
bool Location::hasMethods() const
{
	return m_allowedMethods.first;
}

/**
 *
 * @brief Checks if the location has CGI settings.
 *
 * @return true if CGI settings are present, false otherwise.
 *
 */
bool Location::hasCgi() const
{
	return m_cgi != NULL;
}

/**
 *
 * @brief Checks if file uploads are allowed for the location.
 *
 * @return true if file uploads are allowed, false otherwise.
 *
 */
bool Location::hasUpload() const
{
	return m_upload.first;
}

/**
 *
 * @brief Checks if the location has a return value set.
 *
 * @return true if a return value is set, false otherwise.
 *
 */
bool Location::hasReturn() const
{
	return m_return.first;
}


/**
 *
 * @brief Checks if the location has index files set.
 *
 * @return true if index files are set, false otherwise.
 *
 */
bool Location::hasIndexes() const
{
	return m_indexFiles.first;
}

/**
 *
 * @brief Checks if the location has a maximum body size set.
 *
 * @return true if a maximum body size is set, false otherwise.
 *
 */
bool Location::hasMaxBodySize() const
{
	return m_maxBodySize.first;
}



/**
 *
 * @brief Sets the path of the location.
 *
 * @param path The path to be set.
 *
 */
void Location::setPath(const std::string &path)
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
void Location::setRoot(const std::string &root)
{
	m_root.first  = true;
	m_root.second = root;
}


/**
 *
 * @brief Sets the alias for the location.
 *
 * @param alias The alias to be set.
 *
 */
void Location::setAlias(const std::string &alias)
{
	m_alias.first  = true;
	m_alias.second = alias;
}


/**
 *
 * @brief Sets whether auto-indexing is enabled for the location.
 *
 * @param autoIndex A boolean indicating whether to enable auto-indexing.
 *
 */
void Location::setAutoIndex(const bool autoIndex)
{
	m_autoIndex.first  = true;
	m_autoIndex.second = autoIndex;
}
/**
 *
 * @brief Sets the CGI parameters for the location.
 *
 * @param cgi A pointer to the CgiParams object to be set.
 *
 *
 */
void Location::setCgiParams(const CgiParams *cgi)
{
	m_cgi = cgi;
}


/**
 *
 * @brief Sets the allowed HTTP methods for the location.
 *
 * @param methods A set of HttpMethod objects representing the allowed methods.
 *
 */
void Location::setMethods(const Set<HttpMethod> &methods)
{
	m_allowedMethods.first	= true;
	m_allowedMethods.second = methods;
}

void Location::setMethods(const Vec<std::string> &methods)
{
	m_allowedMethods.first = true;
	for (size_t i = 0; i < methods.size(); ++i)
		m_allowedMethods.second.push(HttpMethod(methods[i]));
}


/**
 *
 * @brief Sets the return value for the location.
 *
 * @param returnVal A LocationReturn object representing the return value to be set.
 *
 */
void Location::setReturn(const LocationReturn &returnVal)
{
	m_return.first	= true;
	m_return.second = returnVal;
}

/**
 *
 * @brief Sets the return value for the location using a vector of strings.
 *
 * @param returnVal A vector of strings representing the return value to be set.
 *
 */
void Location::setReturn(const Vec<std::string> &returnVal)
{
	if (returnVal.empty())
		return;
	m_return.first = true;
	if (returnVal.size() >= 2)
	{
		m_return.second.setStatusCode(Number::toInt(returnVal[0]));
		m_return.second.setTarget(returnVal[1]);
		return;
	}
	m_return.second.setStatusCode(Number::toInt(returnVal[0]));
	m_return.second.setTarget("");
}


/**
 *
 * @brief Sets the list of index files for the location.
 *
 * @param indexFiles A vector of strings representing the index files to be set.
 *
 */
void Location::setIndexes(const Vec<std::string> &indexFiles)
{
	m_indexFiles.first	= true;
	m_indexFiles.second = indexFiles;
}

/**
 *
 * @brief Adds an index file to the location's list of index files.
 *
 * @param indexFile The index file to be added.
 *
 */
void Location::addIndexFile(const std::string &indexFile)
{
	m_indexFiles.first = true;
	m_indexFiles.second.push(indexFile);
}

/**
 *
 * @brief Sets the upload directory for the location using a vector of strings.
 *
 * @param upload A vector containing either 'on' with the upload directory or 'off' to disable uploads.
 *
 */
void Location::setUpload(const Vec<std::string> &upload)
{
	if (upload.empty())
		return;
	m_upload.first = true;
	if (upload[0] == "on" && upload.size() == 2)
	{
		m_upload.second.setOn(true);
		m_upload.second.setUploadDir(upload[1]);
	}
	else if (upload[0] == "off")
	{
		m_upload.second.setOn(false);
		m_upload.second.setUploadDir("");
	}
}


/**
 *
 * @brief Sets the maximum body size for the location.
 *
 * @param maxBodySize A string representing the maximum body size (e.g., "10m", "500k").
 *
 */
void Location::setMaxBodySize(const std::string &maxBodySize)
{
	m_maxBodySize.first	 = true;
	m_maxBodySize.second = Number::toBytes(maxBodySize);
}


#include "Logger.hpp"


std::ostream &operator<<(std::ostream &os, const Location &location)
{
	os << "Location(path: " << location.getPath();
	if (location.hasRoot())
		os << ", root: " << location.getRoot();
	if (location.hasAlias())
		os << ", alias: " << location.getAlias();
	if (location.hasAutoIndex())
		os << ", autoIndex: " << (location.getAutoIndex() ? "true" : "false");
	if (location.hasMethods())
		os << ", allowedMethods: " << Logger::get(location.getAllowedMethods());
	if (location.hasUpload())
		os << ", uploadDir: " << location.getUpload();
	if (location.hasReturn())
		os << ", return: " << Logger::get(location.getReturn());
	if (location.hasIndexes())
		os << ", indexes: " << Logger::get(location.getIndexFiles());
	if (location.hasCgi())
		os << ", cgiParams: " << Logger::get(location.getCgiParams());
	os << ")";
	return os;
}
