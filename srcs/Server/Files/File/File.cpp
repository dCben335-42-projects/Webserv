#include "File.hpp"

#include "Date.hpp"
#include "Number.hpp"
#include "FileManager.hpp"
#include "unistd.h"
#include "sys/stat.h"
#include "Logger.hpp"

File::File(
	const std::string &name,
	const std::string &ressourcePath,
	const std::string &location,
	const std::string &defaultMime)
{
	if (!FileManager::exists(ressourcePath))
		throw FileManager::FileDoesNotExistException(ressourcePath);
	m_stat = FileManager::getStat(ressourcePath);

	m_name				= name;
	m_mode				= m_stat.st_mode;
	m_isDirectory		= S_ISDIR(m_mode);
	m_uri				= String::removeDuplicateSlashes(location + "/" + name + (m_isDirectory ? "/" : ""));
	m_fullPath			= ressourcePath;
	m_size				= static_cast<size_t>(m_stat.st_size);
	m_lastModified		= Date::toHttpDateFormat(gmtime(&m_stat.st_mtime));
	m_createdAtHuman	= Date::toHumanFormat(gmtime(&m_stat.st_ctime));
	m_lastModifiedHuman = Date::toHumanFormat(localtime(&m_stat.st_mtime));

	m_extension = FileManager::determineExtension(name, &m_extensionPos);
	m_sizeHuman = Number::bytesToHumanSize(m_size);
	m_icon		= FileManager::getIcon(*this);
	m_mime		= FileManager::getMime(*this, defaultMime);
	m_isHidden	= (name[0] == '.');
}

File::File(
	const std::string &ressourcePath,
	const std::string &location,
	const std::string &defaultMime)
{
	size_t		lastSlashPos = ressourcePath.rfind('/');
	std::string name		 = (lastSlashPos != std::string::npos)
								   ? ressourcePath.substr(lastSlashPos + 1)
								   : ressourcePath;
	*this					 = File(name, ressourcePath, location, defaultMime);
}

File::~File()
{
}

/**
 *
 * @brief Gets the name of the file.
 *
 * @return A constant reference to the name of the file.
 *
 */
const std::string &File::getName() const
{
	return m_name;
}

/**
 *
 * @brief Gets the URI of the file.
 *
 * @return A constant reference to the URI of the file.
 *
 */
const std::string &File::getUri() const
{
	return m_uri;
}

/**
 *
 * @brief Gets the full path of the file on the server.
 *
 * @return A constant reference to the full path of the file.
 *
 */
const std::string &File::getFullPath() const
{
	return m_fullPath;
}

/**
 *
 * @brief Gets the last modified date of the file in HTTP date format.
 *
 * @return A constant reference to the last modified date of the file.
 *
 */
const std::string &File::getLastModified() const
{
	return m_lastModified;
}

/**
 *
 * @brief Gets the MIME content type of the file.
 *
 * @return A constant reference to the MIME content type of the file.
 *
 */
const std::string &File::getMime() const
{
	return m_mime;
}

/**
 *
 * @brief Gets the human-readable size of the file.
 *
 * @return A constant reference to the human-readable size of the file.
 *
 */
const std::string &File::getSizeHuman() const
{
	return m_sizeHuman;
}

/**
 *
 * @brief Gets the human-readable last modified date of the file.
 *
 * @return A constant reference to the human-readable last modified date of the file.
 *
 */
const std::string &File::getLastModifiedHuman() const
{
	return m_lastModifiedHuman;
}

/**
 *
 * @brief Gets the icon associated with the file type.
 *
 * @return A constant reference to the icon of the file.
 *
 */
const std::string &File::getIcon() const
{
	return m_icon;
}

/**
 *
 * @brief Checks if the file is hidden.
 *
 * @return true if the file is hidden, false otherwise.
 *
 */
bool File::getIsHidden() const
{
	return m_isHidden;
}

/**
 *
 * @brief Checks if the file is a directory.
 *
 * @return true if the file is a directory, false otherwise.
 *
 */
bool File::getIsDirectory() const
{
	return m_isDirectory;
}

/**
 *
 * @brief Checks if the file has read permissions for the user.
 *
 * @return true if the file has read permissions for the user, false otherwise.
 *
 */
bool File::getHasReadPermission() const
{
	return (m_stat.st_mode & S_IRUSR);
}

/**
 *
 * @brief Gets the position of the file extension in the file name.
 *
 * @return The position of the file extension in the file name.
 *
 */
size_t File::getExtensionPos() const
{
	return m_extensionPos;
}

/**
 *
 * @brief Gets the size of the file in bytes.
 *
 * @return The size of the file in bytes.
 *
 */
size_t File::getSize() const
{
	return m_size;
}

/**
 *
 * @brief Gets the file extension, including the dot (e.g., ".txt").
 *
 * @return A constant reference to the file extension, or an empty string if there is no valid extension.
 *
 */
const std::string &File::getExtension() const
{
	return m_extension;
}

/**
 *
 * @brief Gets the human-readable creation date of the file.
 *
 * @return A constant reference to the human-readable creation date of the file.
 *
 */
const std::string &File::getCreatedAtHuman() const
{
	return m_createdAtHuman;
}

/**
 *
 * @brief Gets the content of the file as a string.
 *
 * @return A constant reference to the content of the file.
 *
 */
const std::string &File::getContent() const
{
	return m_content;
}

/**
 *
 * @brief Reads the content of the file and stores it in the m_content member variable.
 *
 * @throws FileHandler::FileReadException if there is an error reading the file.
 *
 */
File &File::read()
{
	m_content = FileManager::readFile(m_fullPath);
	return *this;
}
