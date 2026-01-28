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
#include "fileExtension.hpp"

const Map<std::string> FileHandler::m_iconMap =
	FileHandler::initIconMap();

const Map<std::string> FileHandler::m_extensionTypeMap =
	FileHandler::initExtensionTypeMap();

Map<std::string> FileHandler::initExtensionTypeMap()
{
	Map<std::string> extensionTypeMap;

	extensionTypeMap[FILE_EXT_PLAIN] = MIME_PLAIN;
	extensionTypeMap[FILE_EXT_CSV]	 = MIME_CSV;
	extensionTypeMap[FILE_EXT_HTML]	 = MIME_HTML;
	extensionTypeMap[FILE_EXT_HTM]	 = MIME_HTML;
	extensionTypeMap[FILE_EXT_CSS]	 = MIME_CSS;

	extensionTypeMap[FILE_EXT_JS]	= MIME_JS;
	extensionTypeMap[FILE_EXT_JSON] = MIME_JSON;
	extensionTypeMap[FILE_EXT_EOT]	= MIME_EOT;
	extensionTypeMap[FILE_EXT_PDF]	= MIME_PDF;
	extensionTypeMap[FILE_EXT_XML]	= MIME_XML;

	extensionTypeMap[FILE_EXT_ZIP]	= MIME_ZIP;
	extensionTypeMap[FILE_EXT_YAML] = MIME_YAML;
	extensionTypeMap[FILE_EXT_YML]	= MIME_YAML;
	extensionTypeMap[FILE_EXT_TOML] = MIME_TOML;
	extensionTypeMap[FILE_EXT_RTF]	= MIME_RTF;

	extensionTypeMap[FILE_EXT_PNG]	= MIME_PNG;
	extensionTypeMap[FILE_EXT_JPG]	= MIME_JPEG;
	extensionTypeMap[FILE_EXT_WEBP] = MIME_WEBP;
	extensionTypeMap[FILE_EXT_JPEG] = MIME_JPEG;
	extensionTypeMap[FILE_EXT_GIF]	= MIME_GIF;
	extensionTypeMap[FILE_EXT_SVG]	= MIME_SVG;
	extensionTypeMap[FILE_EXT_AVIF] = MIME_AVIF;
	extensionTypeMap[FILE_EXT_HEIC] = MIME_HEIC;
	extensionTypeMap[FILE_EXT_ICO]	= MIME_ICO;

	extensionTypeMap[FILE_EXT_MP4]	= MIME_MP4;
	extensionTypeMap[FILE_EXT_WEBM] = MIME_WEBM;

	extensionTypeMap[FILE_EXT_TTF]	 = MIME_TTF;
	extensionTypeMap[FILE_EXT_WOFF]	 = MIME_WOFF;
	extensionTypeMap[FILE_EXT_WOFF2] = MIME_WOFF2;

	extensionTypeMap[FILE_EXT_MP3]	= MIME_MP3;
	extensionTypeMap[FILE_EXT_FLAC] = MIME_FLAC;
	extensionTypeMap[FILE_EXT_WAV]	= MIME_WAV;

	return extensionTypeMap;
}

Map<std::string> FileHandler::initIconMap()
{
	Map<std::string> iconMap;

	iconMap[FILE_EXT_PLAIN] = ICON_PLAIN;
	iconMap[FILE_EXT_CSV]	= ICON_PLAIN;
	iconMap[FILE_EXT_HTML]	= ICON_PLAIN;
	iconMap[FILE_EXT_HTM]	= ICON_PLAIN;
	iconMap[FILE_EXT_CSS]	= ICON_PLAIN;
	iconMap[FILE_EXT_JS]	= ICON_PLAIN;
	iconMap[FILE_EXT_JSON]	= ICON_PLAIN;

	iconMap[FILE_EXT_PDF]  = ICON_PDF;
	iconMap[FILE_EXT_XML]  = ICON_PLAIN;
	iconMap[FILE_EXT_ZIP]  = ICON_ARCHIVE;
	iconMap[FILE_EXT_YAML] = ICON_PLAIN;
	iconMap[FILE_EXT_YML]  = ICON_PLAIN;
	iconMap[FILE_EXT_TOML] = ICON_PLAIN;
	iconMap[FILE_EXT_RTF]  = ICON_PLAIN;

	iconMap[FILE_EXT_PNG]  = ICON_IMAGE;
	iconMap[FILE_EXT_JPG]  = ICON_IMAGE;
	iconMap[FILE_EXT_WEBP] = ICON_IMAGE;
	iconMap[FILE_EXT_JPEG] = ICON_IMAGE;
	iconMap[FILE_EXT_GIF]  = ICON_IMAGE;
	iconMap[FILE_EXT_SVG]  = ICON_IMAGE;
	iconMap[FILE_EXT_AVIF] = ICON_IMAGE;
	iconMap[FILE_EXT_HEIC] = ICON_IMAGE;
	iconMap[FILE_EXT_ICO]  = ICON_IMAGE;

	iconMap[FILE_EXT_MP4]  = ICON_VIDEO;
	iconMap[FILE_EXT_WEBM] = ICON_VIDEO;

	iconMap[FILE_EXT_TTF]	= ICON_FONT;
	iconMap[FILE_EXT_WOFF]	= ICON_FONT;
	iconMap[FILE_EXT_WOFF2] = ICON_FONT;
	iconMap[FILE_EXT_EOT]	= ICON_FONT;


	iconMap[FILE_EXT_MP3]  = ICON_AUDIO;
	iconMap[FILE_EXT_FLAC] = ICON_AUDIO;
	iconMap[FILE_EXT_WAV]  = ICON_AUDIO;

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
	return MIME_OCTET;
}

std::string FileHandler::getContentType(const File &file)
{
	std::string ext = file.getExtension();

	Map<std::string>::const_iterator it = m_extensionTypeMap.find(ext);
	if (it != m_extensionTypeMap.end())
		return it->second;
	return MIME_OCTET;
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