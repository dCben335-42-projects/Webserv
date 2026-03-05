#include "Location.hpp"
#include "Number.hpp"
#include "log.hpp"
#include "Logger.hpp"


Location::Location()
{
	reset();
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
	reset();
	m_path = path;
}


Location::Location(const Location &other)
{
	m_path = other.m_path;

	m_root	= other.m_root;
	m_alias = other.m_alias;

	m_autoIndex	   = other.m_autoIndex;
	m_return	   = other.m_return;
	m_upload	   = other.m_upload;
	m_maxBodySize  = other.m_maxBodySize;
	m_authRequired = other.m_authRequired;

	m_addHeaders = other.m_addHeaders;
	m_errorPages = other.m_errorPages;
	m_cgiBin	 = other.m_cgiBin;

	m_indexes = other.m_indexes;
	m_methods = other.m_methods;
}

Location::~Location()
{
}


void Location::reset()
{
	m_path = "";

	m_root	= std::make_pair(false, std::string());
	m_alias = std::make_pair(false, std::string());

	m_autoIndex	   = std::make_pair(false, false);
	m_return	   = std::make_pair(false, LocationReturn());
	m_upload	   = std::make_pair(false, Upload());
	m_maxBodySize  = std::make_pair(false, 0);
	m_authRequired = std::make_pair(false, false);

	m_addHeaders = Map<std::string>();
	m_errorPages = Map<unsigned short, std::string>();
	m_cgiBin	 = Map<std::string>();

	m_indexes = std::make_pair(false, Vec<std::string>());
	m_methods = std::make_pair(false, Set<HttpMethod>());
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
const Vec<std::string> &Location::getIndexes() const
{
	return m_indexes.second;
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
 * @brief Gets the allowed HTTP methods for the location.
 *
 * @return A constant reference to the set of allowed HTTP methods.
 *
 */
const Set<HttpMethod> &Location::getMethods() const
{
	return m_methods.second;
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
 * @brief Gets whether authentication is required for the location.
 *
 * @return true if authentication is required, false otherwise.
 *
 */
bool Location::getAuthRequired() const
{
	return m_authRequired.second;
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
	return m_methods.first;
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
	return m_indexes.first;
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
 * @brief Checks if authentication is required for the location.
 *
 * @return true if authentication is required, false otherwise.
 *
 */
bool Location::hasAuthRequired() const
{
	return m_authRequired.first;
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
 * @brief Sets the allowed HTTP methods for the location using a vector of strings.
 *
 * @param methods A vector of strings representing the HTTP methods to be allowed for the location.
 *
 */
void Location::setMethods(const Vec<std::string> &methods)
{
	m_methods.first = true;
	for (size_t i = 0; i < methods.size(); ++i)
		m_methods.second.push(HttpMethod(methods[i]));
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
	m_indexes.first	 = true;
	m_indexes.second = indexFiles;
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
	const size_t	   lastPos	  = maxBodySize.size() - 1;
	const std::string &sizeNoUint = std::isalpha(maxBodySize.at(lastPos)) ? maxBodySize.substr(0, lastPos) : maxBodySize;
	if (!Number::isNumber(sizeNoUint))
	{
		Logger::custom(ERROR_MSG_MAX_BODY_SIZE_NOT_NUMBER + Logger::get(maxBodySize) + ". This max body size will be ignored.");
		return;
	}
	m_maxBodySize.first	 = true;
	m_maxBodySize.second = Number::toBytes(maxBodySize);
}

/**
 *
 * @brief Sets the maximum body size for the location using a size_t value.
 *
 * @param maxBodySize The maximum body size in bytes to be set for the location.
 *
 */
void Location::setMaxBodySize(const size_t maxBodySize)
{
	m_maxBodySize.first	 = true;
	m_maxBodySize.second = maxBodySize;
}


/**
 *
 * @brief Sets whether authentication is required for the location.
 *
 * @param authRequired A boolean indicating whether authentication is required for the location.
 *
 */
void Location::setAuthRequired(const bool authRequired)
{
	m_authRequired.first  = true;
	m_authRequired.second = authRequired;
}



/**
 *
 * @brief Sets the additional headers for the location.
 *
 * @param addHeaders A map of header names and their corresponding values to be added to the location.
 *
 */
void Location::setAddHeaders(const Map<std::string> &addHeaders)
{
	m_addHeaders = addHeaders;
}

/**
 *
 * @brief Combines the additional headers for the location with the given headers.
 *
 * @param addHeaders A map of header names and their corresponding values to be combined with the existing additional headers of the location.
 *
 * @note If a header already exists in the location's additional headers, it will be overwritten by the value from addHeaders.
 *
 */
void Location::combineAddHeaders(const Map<std::string> &addHeaders)
{
	for (Map<std::string>::const_iterator it = addHeaders.begin(); it != addHeaders.end(); ++it)
		m_addHeaders.set(it->first, it->second);
}

/**
 *
 * @brief Sets the error pages for the location.
 *
 * @param errorPages A map of HTTP status codes and their corresponding error page paths to be set for the location.
 *
 */
void Location::setErrorPages(const Map<std::string> &errorPages)
{
	for (Map<std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it)
		m_errorPages.set(Number::toUShort(it->first), it->second);
}

/**
 *
 * @brief Combines the error pages for the location with the given error pages.
 *
 * @param errorPages A map of HTTP status codes and their corresponding error page paths to be combined with the existing error pages of the location.
 *
 * @note If an error page already exists in the location's error pages, it will be overwritten by the value from errorPages.
 *
 */
void Location::combineErrorPages(const Map<std::string> &errorPages)
{
	for (Map<std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it)
		m_errorPages.set(Number::toUShort(it->first), it->second);
}

/**
 *
 * @brief Sets the CGI passes for the location.
 *
 * @param cgiPasses A map of file extensions and their corresponding CGI script paths to be set for the location.
 *
 */
void Location::setCgiBin(const Map<std::string> &cgiBin)
{
	m_cgiBin = cgiBin;
}
/**
 *
 * @brief Combines the CGI passes for the location with the given CGI passes.
 *
 * @param cgiBin A map of file extensions and their corresponding CGI script paths to be combined with the existing CGI passes of the location.
 *
 * @note If a CGI pass already exists in the location's CGI passes, it will be overwritten by the value from cgiBin.
 *
 */
void Location::combineCgiBin(const Map<std::string> &cgiBin)
{
	for (Map<std::string>::const_iterator it = cgiBin.begin(); it != cgiBin.end(); ++it)
		m_cgiBin.set(it->first, it->second);
}


/**
 *
 * @brief Gets the additional headers for the location.
 *
 * @return A constant reference to a map of header names and their corresponding values that are added to the location.
 *
 */
const Map<std::string> &Location::getAddHeaders() const
{
	return m_addHeaders;
}

/**
 *
 * @brief Gets the error pages for the location.
 *
 * @return A constant reference to a map of HTTP status codes and their corresponding error page paths that are set for the location.
 *
 */
const Map<unsigned short, std::string> &Location::getErrorPages() const
{
	return m_errorPages;
}


/**
 *
 * @brief Checks if there is an error page set for a specific HTTP status code.
 *
 * @param statusCode The HTTP status code to check for an associated error page.
 *
 * @return true if there is an error page set for the given HTTP status code, false otherwise.
 *
 */
bool Location::hasErrorPage(const unsigned short statusCode) const
{
	return m_errorPages.has(statusCode);
}

/**
 *
 * @brief Gets the error page path for a specific HTTP status code.
 *
 * @param statusCode The HTTP status code for which to retrieve the error page path.
 *
 * @return The error page path corresponding to the given HTTP status code, or an empty string if no error page is set for that status code.
 *
 */
std::string Location::getErrorPage(const unsigned short statusCode) const
{
	return m_errorPages.get(statusCode);
}
/**
 *
 * @brief Gets the CGI bin map for the location.
 *
 * @return A constant reference to a map of file extensions and their corresponding CGI script paths that are set for the location.
 *
 */
const Map<std::string> &Location::getCgiBin() const
{
	return m_cgiBin;
}

#include "Logger.hpp"


std::ostream &operator<<(std::ostream &os, const Location &location)
{
	os << "Location(\n";
	Logger::incrementIndent();
	os << Logger::indent() << "path: " << location.getPath() << "\n";

	if (location.hasRoot())
		os << Logger::indent() << "root: " << location.getRoot() << "\n";
	if (location.hasAlias())
		os << Logger::indent() << "alias: " << location.getAlias() << "\n";

	if (location.hasAutoIndex())
		os << Logger::indent() << "autoIndex: " << (location.getAutoIndex() ? "true" : "false") << "\n";
	if (location.hasReturn())
		os << Logger::indent() << "return: " << Logger::get(location.getReturn()) << "\n";
	if (location.hasUpload())
		os << Logger::indent() << "uploadDir: " << location.getUpload() << "\n";
	if (location.hasMaxBodySize())
		os << Logger::indent() << "maxBodySize: " << location.getMaxBodySize() << " bytes" << "\n";
	if (location.hasAuthRequired())
		os << Logger::indent() << "authRequired: " << (location.getAuthRequired() ? "true" : "false") << "\n";

	if (!location.getAddHeaders().empty())
		os << Logger::indent() << "addHeaders: " << Logger::get(location.getAddHeaders()) << "\n";
	if (!location.getErrorPages().empty())
		os << Logger::indent() << "errorPages: " << Logger::get(location.getErrorPages()) << "\n";
	if (!location.getCgiBin().empty())
		os << Logger::indent() << "cgiBin: " << Logger::get(location.getCgiBin()) << "\n";

	if (location.hasMethods())
		os << Logger::indent() << "allowedMethods: " << Logger::get(location.getMethods()) << "\n";
	if (location.hasIndexes())
		os << Logger::indent() << "indexes: " << Logger::get(location.getIndexes()) << "\n";

	Logger::decrementIndent();
	os << Logger::indent() << ")";
	return os;
}
