#include "FileHandler.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>

bool FileHandler::fileExists(const std::string &path) {
    struct stat	st;
    return stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode);
}

std::string FileHandler::readFile(const std::string &path)
{
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1)
        throw FileHandler::FileDoesNotExistException(path);
    const size_t	bufferSize = 4096;
    char			buffer[bufferSize];
    ssize_t			bytesRead = 1;
    std::string		content;

    while (bytesRead > 0)
    {
		bytesRead = read(fd, buffer, bufferSize);
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
	const std::string &reqPath
)
{
    const std::string full = root + "/" + reqPath;
    char resolved[PATH_MAX];
    char resolvedRoot[PATH_MAX];

    if (!realpath(full.c_str(), resolved))
		throw FileHandler::InvalidPathException(reqPath, root);
    if (!realpath(root.c_str(), resolvedRoot))
        throw FileHandler::InvalidPathException(root, "root");

    std::string safe = resolved;
    std::string base = resolvedRoot;
    if (safe.compare(0, base.size(), base) != 0)
        throw FileHandler::InvalidPathException(reqPath, root);
    return (safe);
}
