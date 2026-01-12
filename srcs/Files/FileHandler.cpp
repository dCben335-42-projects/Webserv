#include "FileHandler.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>
#include "Date.hpp"
#include "config.hpp"

const Map<std::string> FileHandler::m_extensionTypeMap =
	FileHandler::initExtensionTypeMap();

Map<std::string> FileHandler::initExtensionTypeMap()
{
	Map<std::string> extensionTypeMap;

	extensionTypeMap[".txt"]  = HEADER_TYPE_PLAIN;
	extensionTypeMap[".csv"]  = HEADER_TYPE_CSV;
	extensionTypeMap[".html"] = HEADER_TYPE_HTML;
	extensionTypeMap[".css"]  = HEADER_TYPE_CSS;

	extensionTypeMap[".js"]	  = HEADER_TYPE_JS;
	extensionTypeMap[".json"] = HEADER_TYPE_JSON;
	extensionTypeMap[".eot"]  = HEADER_TYPE_EOT;
	extensionTypeMap[".pdf"]  = HEADER_TYPE_PDF;
	extensionTypeMap[".xml"]  = HEADER_TYPE_XML;
	extensionTypeMap[".zip"]  = HEADER_TYPE_ZIP;
	extensionTypeMap[".yaml"] = HEADER_TYPE_YAML;
	extensionTypeMap[".yml"]  = HEADER_TYPE_YAML;
	extensionTypeMap[".toml"] = HEADER_TYPE_TOML;
	extensionTypeMap[".rtf"]  = HEADER_TYPE_RTF;

	extensionTypeMap[".png"]  = HEADER_TYPE_PNG;
	extensionTypeMap[".jpg"]  = HEADER_TYPE_JPEG;
	extensionTypeMap[".webp"] = HEADER_TYPE_WEBP;
	extensionTypeMap[".jpeg"] = HEADER_TYPE_JPEG;
	extensionTypeMap[".gif"]  = HEADER_TYPE_GIF;
	extensionTypeMap[".svg"]  = HEADER_TYPE_SVG;
	extensionTypeMap[".avif"] = HEADER_TYPE_AVIF;
	extensionTypeMap[".heic"] = HEADER_TYPE_HEIC;
	extensionTypeMap[".ico"]  = HEADER_TYPE_ICO;


	extensionTypeMap[".mp4"]  = HEADER_TYPE_MP4;
	extensionTypeMap[".webm"] = HEADER_TYPE_WEBM;


	extensionTypeMap[".ttf"]   = HEADER_TYPE_TTF;
	extensionTypeMap[".woff"]  = HEADER_TYPE_WOFF;
	extensionTypeMap[".woff2"] = HEADER_TYPE_WOFF2;

	extensionTypeMap[".mp3"]  = HEADER_TYPE_MP3;
	extensionTypeMap[".flac"] = HEADER_TYPE_FLAC;
	extensionTypeMap[".wav"]  = HEADER_TYPE_WAV;

	return extensionTypeMap;
}
bool FileHandler::fileExists(const std::string &path)
{
	struct stat st;
	return stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode);
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

std::string FileHandler::findLastModifiedDate(const std::string &filePath)
{
	struct stat fileStat;
	if (stat(filePath.c_str(), &fileStat) != 0)
		throw FileHandler::FileDoesNotExistException(filePath);
	return Date::toHttpDateFormat(gmtime(&fileStat.st_mtime));
}