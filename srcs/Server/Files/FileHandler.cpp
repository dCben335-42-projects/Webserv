#include "FileHandler.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "Date.hpp"

#include <limits.h>
#include "webserv.hpp"
#include "httpHeaders.hpp"
#include "httpMimes.hpp"

const Map<std::string> FileHandler::m_iconMap =
	FileHandler::initIconMap();

const Map<std::string> FileHandler::m_extensionTypeMap =
	FileHandler::initExtensionTypeMap();

Map<std::string> FileHandler::initExtensionTypeMap()
{
	Map<std::string> extensionTypeMap;

	extensionTypeMap[HTTP_EXT_PLAIN] = HEADER_TYPE_PLAIN;
	extensionTypeMap[HTTP_EXT_CSV]	 = HEADER_TYPE_CSV;
	extensionTypeMap[HTTP_EXT_HTML]	 = HEADER_TYPE_HTML;
	extensionTypeMap[HTTP_EXT_HTM]	 = HEADER_TYPE_HTML;
	extensionTypeMap[HTTP_EXT_CSS]	 = HEADER_TYPE_CSS;

	extensionTypeMap[HTTP_EXT_JS]	= HEADER_TYPE_JS;
	extensionTypeMap[HTTP_EXT_JSON] = HEADER_TYPE_JSON;
	extensionTypeMap[HTTP_EXT_EOT]	= HEADER_TYPE_EOT;
	extensionTypeMap[HTTP_EXT_PDF]	= HEADER_TYPE_PDF;
	extensionTypeMap[HTTP_EXT_XML]	= HEADER_TYPE_XML;

	extensionTypeMap[HTTP_EXT_ZIP]	= HEADER_TYPE_ZIP;
	extensionTypeMap[HTTP_EXT_YAML] = HEADER_TYPE_YAML;
	extensionTypeMap[HTTP_EXT_YML]	= HEADER_TYPE_YAML;
	extensionTypeMap[HTTP_EXT_TOML] = HEADER_TYPE_TOML;
	extensionTypeMap[HTTP_EXT_RTF]	= HEADER_TYPE_RTF;

	extensionTypeMap[HTTP_EXT_PNG]	= HEADER_TYPE_PNG;
	extensionTypeMap[HTTP_EXT_JPG]	= HEADER_TYPE_JPEG;
	extensionTypeMap[HTTP_EXT_WEBP] = HEADER_TYPE_WEBP;
	extensionTypeMap[HTTP_EXT_JPEG] = HEADER_TYPE_JPEG;
	extensionTypeMap[HTTP_EXT_GIF]	= HEADER_TYPE_GIF;
	extensionTypeMap[HTTP_EXT_SVG]	= HEADER_TYPE_SVG;
	extensionTypeMap[HTTP_EXT_AVIF] = HEADER_TYPE_AVIF;
	extensionTypeMap[HTTP_EXT_HEIC] = HEADER_TYPE_HEIC;
	extensionTypeMap[HTTP_EXT_ICO]	= HEADER_TYPE_ICO;

	extensionTypeMap[HTTP_EXT_MP4]	= HEADER_TYPE_MP4;
	extensionTypeMap[HTTP_EXT_WEBM] = HEADER_TYPE_WEBM;

	extensionTypeMap[HTTP_EXT_TTF]	 = HEADER_TYPE_TTF;
	extensionTypeMap[HTTP_EXT_WOFF]	 = HEADER_TYPE_WOFF;
	extensionTypeMap[HTTP_EXT_WOFF2] = HEADER_TYPE_WOFF2;

	extensionTypeMap[HTTP_EXT_MP3]	= HEADER_TYPE_MP3;
	extensionTypeMap[HTTP_EXT_FLAC] = HEADER_TYPE_FLAC;
	extensionTypeMap[HTTP_EXT_WAV]	= HEADER_TYPE_WAV;

	return extensionTypeMap;
}

Map<std::string> FileHandler::initIconMap()
{
	Map<std::string> iconMap;

	iconMap[HTTP_EXT_PLAIN] = ICON_PLAIN;
	iconMap[HTTP_EXT_CSV]	= ICON_PLAIN;
	iconMap[HTTP_EXT_HTML]	= ICON_PLAIN;
	iconMap[HTTP_EXT_HTM]	= ICON_PLAIN;
	iconMap[HTTP_EXT_CSS]	= ICON_PLAIN;
	iconMap[HTTP_EXT_JS]	= ICON_PLAIN;
	iconMap[HTTP_EXT_JSON]	= ICON_PLAIN;

	iconMap[HTTP_EXT_PDF]  = ICON_PDF;
	iconMap[HTTP_EXT_XML]  = ICON_PLAIN;
	iconMap[HTTP_EXT_ZIP]  = ICON_ARCHIVE;
	iconMap[HTTP_EXT_YAML] = ICON_PLAIN;
	iconMap[HTTP_EXT_YML]  = ICON_PLAIN;
	iconMap[HTTP_EXT_TOML] = ICON_PLAIN;
	iconMap[HTTP_EXT_RTF]  = ICON_PLAIN;

	iconMap[HTTP_EXT_PNG]  = ICON_IMAGE;
	iconMap[HTTP_EXT_JPG]  = ICON_IMAGE;
	iconMap[HTTP_EXT_WEBP] = ICON_IMAGE;
	iconMap[HTTP_EXT_JPEG] = ICON_IMAGE;
	iconMap[HTTP_EXT_GIF]  = ICON_IMAGE;
	iconMap[HTTP_EXT_SVG]  = ICON_IMAGE;
	iconMap[HTTP_EXT_AVIF] = ICON_IMAGE;
	iconMap[HTTP_EXT_HEIC] = ICON_IMAGE;
	iconMap[HTTP_EXT_ICO]  = ICON_IMAGE;

	iconMap[HTTP_EXT_MP4]  = ICON_VIDEO;
	iconMap[HTTP_EXT_WEBM] = ICON_VIDEO;

	iconMap[HTTP_EXT_TTF]	= ICON_FONT;
	iconMap[HTTP_EXT_WOFF]	= ICON_FONT;
	iconMap[HTTP_EXT_WOFF2] = ICON_FONT;
	iconMap[HTTP_EXT_EOT]	= ICON_FONT;


	iconMap[HTTP_EXT_MP3]  = ICON_AUDIO;
	iconMap[HTTP_EXT_FLAC] = ICON_AUDIO;
	iconMap[HTTP_EXT_WAV]  = ICON_AUDIO;

	iconMap["/"] = ICON_DIRECTORY;
	iconMap[""]	 = ICON_DEFAULT;
	return iconMap;
}



bool FileHandler::fileExists(const std::string &path)
{
	struct stat st;
	return stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode);
}

bool FileHandler::folderExists(const std::string &path)
{
	struct stat st;
	return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

bool FileHandler::hasReadPermission(const std::string &path)
{
	struct stat st;
	if (stat(path.c_str(), &st) != 0)
		return false;
	return (st.st_mode & S_IRUSR);
}

std::string FileHandler::readFile(const std::string &path)
{
	if (!fileExists(path))
		throw FileHandler::FileDoesNotExistException(path);
	if (!hasReadPermission(path))
		throw FileHandler::FilePermissionException(path);
	int fd = open(path.c_str(), O_RDONLY);
	if (fd == -1)
		throw FileHandler::FileDoesNotExistException(path);
	char		buffer[READ_BUFFER_SIZE];
	ssize_t		bytesRead = 1;
	std::string content;

	while (bytesRead > 0)
	{
		bytesRead = read(fd, buffer, READ_BUFFER_SIZE);
		if (bytesRead == -1)
		{
			close(fd);
			throw FileHandler::FileReadException(path);
		}
		content.append(buffer, bytesRead);
	}
	close(fd);
	return (content);
}


std::string FileHandler::getSafePath(
	const std::string &root,
	const std::string &reqPath)
{
	const std::string full = root + "/" + reqPath;
	char			  resolved[PATH_MAX];
	char			  resolvedRoot[PATH_MAX];

	if (!realpath(full.c_str(), resolved))
		throw FileHandler::InvalidPathException(reqPath, root);
	if (!realpath(root.c_str(), resolvedRoot))
		throw FileHandler::InvalidPathException(root, "root");

	const std::string safe = resolved;
	const std::string base = resolvedRoot;
	if (safe.compare(0, base.size(), base) != 0)
		throw FileHandler::InvalidPathException(reqPath, root);
	return (safe);
}


std::string FileHandler::getContentType(const std::string &filePath)
{
	size_t		dotPos = filePath.rfind('.');
	std::string ext;

	if (dotPos != std::string::npos)
		ext = filePath.substr(dotPos);
	else
		ext = "";

	Map<std::string>::const_iterator it = m_extensionTypeMap.find(ext);
	if (it != m_extensionTypeMap.end())
		return it->second;
	return HEADER_TYPE_OCTET;
}

std::string FileHandler::getContentType(const File &file)
{
	std::string ext = file.getExtension();

	Map<std::string>::const_iterator it = m_extensionTypeMap.find(ext);
	if (it != m_extensionTypeMap.end())
		return it->second;
	return HEADER_TYPE_OCTET;
}

std::string FileHandler::getIcon(const File &file)
{
	if (file.getIsDirectory())
		return m_iconMap.get("/");

	std::string ext = file.getExtension();
	if (ext.empty())
		return m_iconMap.get("");

	Map<std::string>::const_iterator it = m_iconMap.find(ext);
	if (it != m_iconMap.end())
		return it->second;
	return m_iconMap.get("");
}

std::string FileHandler::findLastModifiedDate(const std::string &filePath)
{
	struct stat fileStat;
	if (stat(filePath.c_str(), &fileStat) != 0)
		throw FileHandler::FileDoesNotExistException(filePath);
	return Date::toHttpDateFormat(gmtime(&fileStat.st_mtime));
}