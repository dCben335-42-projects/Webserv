#include "File.hpp"
#include "Date.hpp"
#include "Number.hpp"
#include "FileHandler.hpp"

File::File(
	const std::string &name,
	const std::string &path,
	const std::string &root)
{
	const std::string cleanPath = String::removeDuplicateSlashes(path);
	std::string		  fullPath	= root + cleanPath;

	struct stat fileStat;

	if (stat(fullPath.c_str(), &fileStat) != 0)
		throw std::runtime_error("Failed to get file stats for: " + fullPath);

	m_name		   = name;
	m_path		   = cleanPath;
	m_fullPath	   = fullPath;
	m_size		   = static_cast<size_t>(fileStat.st_size);
	m_isDirectory  = S_ISDIR(fileStat.st_mode);
	m_lastModified = Date::toHttpDateFormat(gmtime(&fileStat.st_mtime));

	m_lastModified		= Date::toHttpDateFormat(gmtime(&fileStat.st_mtime));
	m_lastModifiedHuman = Date::toHumanFormat(localtime(&fileStat.st_mtime));

	determineExtension();
	m_sizeHuman	  = Number::byteToHumanSize(m_size);
	m_icon		  = FileHandler::getIcon(*this);
	m_contentType = FileHandler::getContentType(fullPath);
	m_isHidden	  = (name[0] == '.');
}

File::~File()
{
}


std::string File::getName() const
{
	return m_name;
}
std::string File::getPath() const
{
	return m_path;
}
std::string File::getFullPath() const
{
	return m_fullPath;
}
size_t File::getSize() const
{
	return m_size;
}
std::string File::getLastModified() const
{
	return m_lastModified;
}
std::string File::getContent() const
{
	return m_content;
}
bool File::getIsDirectory() const
{
	return m_isDirectory;
}
std::string File::getMimeType() const
{
	return m_contentType;
}
std::string File::getSizeHuman() const
{
	return m_sizeHuman;
}
std::string File::getLastModifiedHuman() const
{
	return m_lastModifiedHuman;
}
std::string File::getIcon() const
{
	return m_icon;
}
bool File::getIsHidden() const
{
	return m_isHidden;
}
std::string File::getExtension() const
{
	return m_extension;
}
size_t File::getExtensionPos() const
{
	return m_extensionPos;
}


void File::determineExtension()
{
	size_t dotPos  = m_name.rfind('.');
	m_extensionPos = dotPos;
	if (dotPos != std::string::npos && dotPos != 0 && dotPos != m_name.length() - 1)
		m_extension = m_name.substr(dotPos);
	else
		m_extension = "";
}
