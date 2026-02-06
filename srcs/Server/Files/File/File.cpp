#include "File.hpp"

#include "Date.hpp"
#include "Number.hpp"
#include "FileHandler.hpp"

File::File(
	const std::string &name,
	const std::string &ressourcePath,
	const std::string &location)
{
	struct stat fileStat;
	if (stat(ressourcePath.c_str(), &fileStat) != 0)
		throw FileHandler::FileDoesNotExistException(ressourcePath);

	m_name				= name;
	m_isDirectory		= S_ISDIR(fileStat.st_mode);
	m_uri				= String::removeDuplicateSlashes(location + "/" + name + (m_isDirectory ? "/" : ""));
	m_fullPath			= ressourcePath;
	m_size				= static_cast<size_t>(fileStat.st_size);
	m_lastModified		= Date::toHttpDateFormat(gmtime(&fileStat.st_mtime));
	m_createdAtHuman	= Date::toHumanFormat(gmtime(&fileStat.st_b));
	m_lastModifiedHuman = Date::toHumanFormat(gmtime(&fileStat.st_mtime));

	m_extension	  = determineExtension(name, &m_extensionPos);
	m_sizeHuman	  = Number::bytesToHumanSize(m_size);
	m_icon		  = FileHandler::getIcon(*this);
	m_contentType = FileHandler::getContentType(ressourcePath);
	m_isHidden	  = (name[0] == '.');
}

File::~File()
{
}


std::string File::getName() const
{
	return m_name;
}
std::string File::getUri() const
{
	return m_uri;
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
std::string File::getCreatedAtHuman() const
{
	return m_createdAtHuman;
}


std::string File::determineExtension(const std::string &fileName, size_t *extPos)
{
	size_t dotPos = fileName.rfind('.');
	if (extPos)
		*extPos = dotPos;
	if (dotPos != std::string::npos && dotPos != 0 && dotPos != fileName.length() - 1)
		return fileName.substr(dotPos);
	return "";
}
