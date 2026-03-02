#include "FileManager.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include <fstream>
#include <sstream>

#include <cstdio>

#include <limits.h>
#include "webserv.hpp"
#include "httpHeaders.hpp"
#include "httpMimes.hpp"
#include "fileExtension.hpp"

const Map<std::string> FileManager::m_icons =
	FileManager::initIcons();

const Map<std::string> FileManager::m_mimes =
	FileManager::initMimes();

Map<std::string> FileManager::initMimes()
{
	Map<std::string> mimes;

	mimes[FILE_EXT_PLAIN] = MIME_TEXT;
	mimes[FILE_EXT_CSV]	  = MIME_CSV;
	mimes[FILE_EXT_HTML]  = MIME_HTML;
	mimes[FILE_EXT_HTM]	  = MIME_HTML;
	mimes[FILE_EXT_CSS]	  = MIME_CSS;
	mimes[FILE_EXT_JS]	  = MIME_JS;
	mimes[FILE_EXT_JSON]  = MIME_JSON;
	mimes[FILE_EXT_XML]	  = MIME_XML;
	mimes[FILE_EXT_MD]	  = MIME_MARKDOWN;
	mimes[FILE_EXT_YAML]  = MIME_YAML;
	mimes[FILE_EXT_YML]	  = MIME_YAML;
	mimes[FILE_EXT_TOML]  = MIME_TOML;
	mimes[FILE_EXT_RTF]	  = MIME_RTF;

	mimes[FILE_EXT_C]		 = MIME_C;
	mimes[FILE_EXT_H]		 = MIME_C;
	mimes[FILE_EXT_CPP]		 = MIME_CPP;
	mimes[FILE_EXT_CC]		 = MIME_CPP;
	mimes[FILE_EXT_HPP]		 = MIME_CPP;
	mimes[FILE_EXT_CS]		 = MIME_CSHARP;
	mimes[FILE_EXT_GO]		 = MIME_GO;
	mimes[FILE_EXT_RS]		 = MIME_RUST;
	mimes[FILE_EXT_PY]		 = MIME_PYTHON;
	mimes[FILE_EXT_JAVA]	 = MIME_JAVA;
	mimes[FILE_EXT_SH]		 = MIME_SHELL;
	mimes[FILE_EXT_MAKEFILE] = MIME_MAKEFILE;

	mimes[FILE_EXT_ZIP]	   = MIME_ZIP;
	mimes[FILE_EXT_GZIP]   = MIME_GZIP;
	mimes[FILE_EXT_TAR]	   = MIME_TAR;
	mimes[FILE_EXT_7Z]	   = MIME_7Z;
	mimes[FILE_EXT_RAR]	   = MIME_RAR;
	mimes[FILE_EXT_OCTET]  = MIME_OCTET;
	mimes[FILE_EXT_EXE]	   = MIME_OCTET;
	mimes[FILE_EXT_WASM]   = MIME_WASM;
	mimes[FILE_EXT_SQLITE] = MIME_SQLITE;
	mimes[FILE_EXT_DB]	   = MIME_SQLITE;

	mimes[FILE_EXT_PNG]	 = MIME_PNG;
	mimes[FILE_EXT_JPG]	 = MIME_JPEG;
	mimes[FILE_EXT_JPEG] = MIME_JPEG;
	mimes[FILE_EXT_WEBP] = MIME_WEBP;
	mimes[FILE_EXT_GIF]	 = MIME_GIF;
	mimes[FILE_EXT_SVG]	 = MIME_SVG;
	mimes[FILE_EXT_AVIF] = MIME_AVIF;
	mimes[FILE_EXT_HEIC] = MIME_HEIC;
	mimes[FILE_EXT_BMP]	 = MIME_BMP;
	mimes[FILE_EXT_TIFF] = MIME_TIFF;
	mimes[FILE_EXT_ICO]	 = MIME_ICO;
	mimes[FILE_EXT_XPM]	 = MIME_XPM;
	mimes[FILE_EXT_ICNS] = MIME_ICNS;

	mimes[FILE_EXT_MP4]	 = MIME_MP4;
	mimes[FILE_EXT_WEBM] = MIME_WEBM;
	mimes[FILE_EXT_OGV]	 = MIME_OGG_VIDEO;
	mimes[FILE_EXT_AVI]	 = MIME_AVI;
	mimes[FILE_EXT_MKV]	 = MIME_MKV;

	mimes[FILE_EXT_MP3]	 = MIME_MP3;
	mimes[FILE_EXT_FLAC] = MIME_FLAC;
	mimes[FILE_EXT_WAV]	 = MIME_WAV;
	mimes[FILE_EXT_OGG]	 = MIME_OGG_AUDIO;
	mimes[FILE_EXT_AAC]	 = MIME_AAC;

	mimes[FILE_EXT_TTF]	  = MIME_TTF;
	mimes[FILE_EXT_WOFF]  = MIME_WOFF;
	mimes[FILE_EXT_WOFF2] = MIME_WOFF2;
	mimes[FILE_EXT_OTF]	  = MIME_OTF;
	mimes[FILE_EXT_EOT]	  = MIME_EOT;

	mimes["/"] = MIME_OCTET;
	mimes[""]  = MIME_OCTET;



	return mimes;
}



Map<std::string> FileManager::initIcons()
{
	Map<std::string> iconMap;

	iconMap[FILE_EXT_PLAIN]	 = ICON_PLAIN;
	iconMap[FILE_EXT_CSV]	 = ICON_PLAIN;
	iconMap[FILE_EXT_HTML]	 = ICON_PLAIN;
	iconMap[FILE_EXT_HTM]	 = ICON_PLAIN;
	iconMap[FILE_EXT_CSS]	 = ICON_PLAIN;
	iconMap[FILE_EXT_JS]	 = ICON_PLAIN;
	iconMap[FILE_EXT_JSON]	 = ICON_PLAIN;
	iconMap[FILE_EXT_XML]	 = ICON_PLAIN;
	iconMap[FILE_EXT_MD]	 = ICON_PLAIN;
	iconMap[FILE_EXT_YAML]	 = ICON_PLAIN;
	iconMap[FILE_EXT_YML]	 = ICON_PLAIN;
	iconMap[FILE_EXT_TOML]	 = ICON_PLAIN;
	iconMap[FILE_EXT_RTF]	 = ICON_PLAIN;
	iconMap[FILE_EXT_SQLITE] = ICON_BINARY;

	iconMap[FILE_EXT_C]		   = ICON_CODE;
	iconMap[FILE_EXT_H]		   = ICON_CODE;
	iconMap[FILE_EXT_CPP]	   = ICON_CODE;
	iconMap[FILE_EXT_CC]	   = ICON_CODE;
	iconMap[FILE_EXT_HPP]	   = ICON_CODE;
	iconMap[FILE_EXT_CS]	   = ICON_CODE;
	iconMap[FILE_EXT_GO]	   = ICON_CODE;
	iconMap[FILE_EXT_RS]	   = ICON_CODE;
	iconMap[FILE_EXT_PY]	   = ICON_CODE;
	iconMap[FILE_EXT_JAVA]	   = ICON_CODE;
	iconMap[FILE_EXT_SH]	   = ICON_CODE;
	iconMap[FILE_EXT_MAKEFILE] = ICON_CODE;

	iconMap[FILE_EXT_ZIP]	= ICON_ARCHIVE;
	iconMap[FILE_EXT_GZIP]	= ICON_ARCHIVE;
	iconMap[FILE_EXT_TAR]	= ICON_ARCHIVE;
	iconMap[FILE_EXT_7Z]	= ICON_ARCHIVE;
	iconMap[FILE_EXT_RAR]	= ICON_ARCHIVE;
	iconMap[FILE_EXT_OCTET] = ICON_BINARY;
	iconMap[FILE_EXT_EXE]	= ICON_BINARY;
	iconMap[FILE_EXT_WASM]	= ICON_BINARY;

	iconMap[FILE_EXT_PNG]  = ICON_IMAGE;
	iconMap[FILE_EXT_JPG]  = ICON_IMAGE;
	iconMap[FILE_EXT_JPEG] = ICON_IMAGE;
	iconMap[FILE_EXT_WEBP] = ICON_IMAGE;
	iconMap[FILE_EXT_GIF]  = ICON_IMAGE;
	iconMap[FILE_EXT_SVG]  = ICON_IMAGE;
	iconMap[FILE_EXT_AVIF] = ICON_IMAGE;
	iconMap[FILE_EXT_HEIC] = ICON_IMAGE;
	iconMap[FILE_EXT_BMP]  = ICON_IMAGE;
	iconMap[FILE_EXT_TIFF] = ICON_IMAGE;
	iconMap[FILE_EXT_ICO]  = ICON_IMAGE;
	iconMap[FILE_EXT_XPM]  = ICON_IMAGE;
	iconMap[FILE_EXT_ICNS] = ICON_IMAGE;

	iconMap[FILE_EXT_MP4]  = ICON_VIDEO;
	iconMap[FILE_EXT_WEBM] = ICON_VIDEO;
	iconMap[FILE_EXT_OGV]  = ICON_VIDEO;
	iconMap[FILE_EXT_AVI]  = ICON_VIDEO;
	iconMap[FILE_EXT_MKV]  = ICON_VIDEO;

	iconMap[FILE_EXT_MP3]  = ICON_AUDIO;
	iconMap[FILE_EXT_FLAC] = ICON_AUDIO;
	iconMap[FILE_EXT_WAV]  = ICON_AUDIO;
	iconMap[FILE_EXT_OGG]  = ICON_AUDIO;
	iconMap[FILE_EXT_AAC]  = ICON_AUDIO;

	iconMap[FILE_EXT_TTF]	= ICON_FONT;
	iconMap[FILE_EXT_WOFF]	= ICON_FONT;
	iconMap[FILE_EXT_WOFF2] = ICON_FONT;
	iconMap[FILE_EXT_OTF]	= ICON_FONT;
	iconMap[FILE_EXT_EOT]	= ICON_FONT;

	iconMap["/"] = ICON_DIRECTORY;
	iconMap[""]	 = ICON_DEFAULT;

	return iconMap;
}


/**
 *
 * @brief Checks if a file or folder exists at the given path.
 *
 * @param path The path to the file or folder to be checked.
 *
 * @return true if a file or folder exists at the given path, false otherwise.
 *
 */
bool FileManager::exists(const std::string &path)
{
	return access(path.c_str(), F_OK) == 0;
}

/**
 *
 * @brief Retrieves the stat structure for a file or folder at the given path.
 *
 * @param path The path to the file or folder for which to retrieve the stat structure.
 *
 * @return The stat structure containing information about the file or folder.
 *
 * @throws FileDoesNotExistException if the file or folder does not exist at the given path.
 *
 */
struct stat FileManager::getStat(const std::string &path)
{
	struct stat st;
	if (stat(path.c_str(), &st) == -1)
		throw FileManager::FileDoesNotExistException(path);
	return st;
}


/**
 *
 * @brief Checks if a file exists at the given path.
 *
 * @param st The stat structure containing information about the file or folder to be checked.
 *
 * @return true if the file exists and is a regular file, false otherwise.
 *
 */
bool FileManager::isFile(struct stat &st)
{
	return S_ISREG(st.st_mode);
}

/**
 *
 * @brief Checks if a file exists at the given path.
 *
 * @param path The path to the file to be checked.
 *
 * @return true if the file exists and is a regular file, false otherwise.
 *
 * @throws FileDoesNotExistException if the file does not exist at the given path.
 *
 */
bool FileManager::isFile(const std::string &path)
{
	struct stat st = getStat(path);
	return isFile(st);
}

/**
 *
 * @brief Checks if a folder exists at the given path.
 *
 * @param st The stat structure containing information about the file or folder to be checked.
 *
 * @return true if the folder exists and is a directory, false otherwise.
 *
 */
bool FileManager::isFolder(struct stat &st)
{
	return S_ISDIR(st.st_mode);
}

/** @brief Checks if a folder exists at the given path.
 *
 * @param path The path to the folder to be checked.
 *
 * @return true if the folder exists and is a directory, false otherwise.
 *
 * @throws FileDoesNotExistException if the folder does not exist at the given path.
 *
 */
bool FileManager::isFolder(const std::string &path)
{
	struct stat st = getStat(path);
	return isFolder(st);
}


/**
 *
 * @brief Checks if the file at the given path has read permissions for the user.
 *
 * @param path The path to the file to be checked.
 *
 * @return true if the file has read permissions for the user, false otherwise.
 *
 */
bool FileManager::hasRPerm(const std::string &path)
{
	return access(path.c_str(), R_OK) == 0;
}

/**
 *
 * @brief Checks if the file at the given path has write permissions for the user.
 *
 * @param path The path to the file to be checked.
 *
 * @return true if the file has write permissions for the user, false otherwise.
 *
 */
bool FileManager::hasWPerm(const std::string &path)
{
	return access(path.c_str(), W_OK) == 0;
}


/**
 *
 * @brief Checks if the file at the given path has execute permissions for the user.
 *
 * @param path The path to the file to be checked.
 *
 * @return true if the file has execute permissions for the user, false otherwise.
 *
 */
bool FileManager::hasXPerm(const std::string &path)
{
	return access(path.c_str(), X_OK) == 0;
}

/**
 *
 * @brief Checks if the file at the given path has read and write permissions for the user.
 *
 * @param path The path to the file to be checked.
 *
 * @return true if the file has read and write permissions for the user, false otherwise.
 *
 */
bool FileManager::hasRWPerm(const std::string &path)
{
	return access(path.c_str(), R_OK | W_OK) == 0;
}


/**
 *
 * @brief Checks if the file at the given path has read, write, and execute permissions for the user.
 *
 * @param path The path to the file to be checked.
 *
 * @return true if the file has read, write, and execute permissions for the user, false otherwise.
 *
 */
bool FileManager::hasRWXPerm(const std::string &path)
{
	return access(path.c_str(), R_OK | W_OK | X_OK) == 0;
}


/**
 *
 * @brief Reads the content of a file at the given path.
 *
 * @param path The path to the file to be read.
 *
 * @return The content of the file as a string.
 *
 * @throws FileDoesNotExistException if the file does not exist.
 * @throws FilePermissionException if the file cannot be read due to permission issues.
 * @throws FileReadException if there is an error while reading the file.
 *
 */
std::string FileManager::readFile(const std::string &path)
{
	if (!exists(path))
		throw FileManager::FileDoesNotExistException(path);
	if (!hasRPerm(path))
		throw FileManager::FilePermissionException(path);

	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open())
		throw FileManager::FileReadException(path);
	std::ostringstream content;
	content << file.rdbuf();

	if (file.fail() && !file.eof())
		throw FileManager::FileReadException(path);
	return content.str();
}


/**
 *
 * @brief Gets the MIME content type for a given file path based on its extension.
 *
 * @param filePath The path to the file for which to determine the content type.
 *
 * @return The MIME content type as a string. If the file extension is not recognized, returns "application/octet-stream".
 *
 */
std::string FileManager::getMime(const std::string &filePath)
{
	const std::string &ext = determineExtension(filePath, NULL);

	if (m_mimes.has(ext))
		return m_mimes.get(ext);
	return MIME_OCTET;
}


/**
 *
 * @brief Gets the MIME content type for a given file based on its extension.
 *
 * @param file The File object for which to determine the content type.
 *
 * @return The MIME content type as a string. If the file extension is not recognized, returns "application/octet-stream".
 *
 */
std::string FileManager::getMime(const File &file)
{
	const std::string &ext = file.getExtension();

	if (m_mimes.has(ext))
		return m_mimes.get(ext);
	return MIME_OCTET;
}


/**
 *
 * @brief Gets the icon path for a given file based on its extension.
 *
 * @param file The File object for which to determine the icon.
 *
 * @return The icon path as a string.
 * If the file extension is not recognized, returns the default icon path.
 *
 */
std::string FileManager::getIcon(const File &file)
{
	if (file.getIsDirectory())
		return m_icons.get("/");

	const std::string &ext = file.getExtension();
	if (ext.empty())
		return m_icons.get("");
	if (m_icons.has(ext))
		return m_icons.get(ext);
	return m_icons.get("");
}



/**
 *
 * @brief Determines the file extension from the given file name.
 *
 * This function finds the last dot in the file name and returns the substring
 * from that dot to the end of the string as the extension.
 *
 * @note If there is no dot, or if the dot is at the beginning or end of the file name,
 * the function returns an empty string, indicating that there is no valid extension.
 *
 * @param fileName The name of the file from which to determine the extension.
 * @param extPos Optional pointer to store the position of the extension in the file name.
 *
 * @return The file extension, including the dot (e.g., ".txt"), or an
 * empty string if there is no valid extension.
 *
 */
std::string FileManager::determineExtension(const std::string &fileName, size_t *extPos)
{
	size_t dotPos = fileName.rfind('.');
	if (extPos)
		*extPos = dotPos;
	if (dotPos != std::string::npos && dotPos != 0 && dotPos != fileName.length() - 1)
		return String::toLower(fileName.substr(dotPos));
	return "";
}

/**
 *
 * @brief Creates a disk file and fills it with the given content
 *
 * @param fullPath The path of the file that should be created and filled
 * @param body The content to write to the file
 *
 * @throw Http500Exception in case open() fails
 */
void FileManager::createDiskFile(const std::string &fullPath, const std::string &body)
{
	std::ofstream file;
	file.open(fullPath.c_str(), std::ofstream::trunc);
	if (!file.is_open())
		throw FileOpenException(fullPath);
	file << body;
	file.close();
}

/**
 *
 * @brief Delete a file using std::remove()
 *
 * @throw FileDeleteException() if std::remove() fails
 */
void FileManager::deleteFile(const char *fullPath)
{
	if (!FileManager::exists(fullPath))
		throw FileDoesNotExistException(fullPath);
	if (FileManager::isFolder(fullPath))
		throw FilePermissionException(fullPath);
	if (!FileManager::hasRWPerm(fullPath))
		throw FilePermissionException(fullPath);
	if (std::remove(fullPath) == -1)
		throw FileDeleteException(fullPath);
}
