#ifndef FILE_MANAGER_HPP
#define FILE_MANAGER_HPP

#include "Map.hpp"
#include <exception>
#include "httpExceptions.hpp"
#include "File.hpp"

#define ICON_PLAIN "📄"
#define ICON_IMAGE "🖼️"
#define ICON_ARCHIVE "📦"
#define ICON_PDF "📕"
#define ICON_VIDEO "🎥"
#define ICON_AUDIO "🎵"
#define ICON_FONT "📝"
#define ICON_DIRECTORY "📂"
#define ICON_BINARY "⚙️"
#define ICON_CODE "💻"
#define ICON_DEFAULT "📄"


class FileManager
{
	private:
		FileManager();
		~FileManager();

		static const Map<std::string> m_mimes;
		static Map<std::string>		  initMimes();

		static const Map<std::string> m_icons;
		static Map<std::string>		  initIcons();

	public:
		static struct stat getStat(const std::string &path);

		static bool isFile(struct stat &st);
		static bool isFile(const std::string &path);
		static bool isFolder(struct stat &st);
		static bool isFolder(const std::string &path);

		static bool exists(const std::string &path);
		static bool hasRPerm(const std::string &path);
		static bool hasWPerm(const std::string &path);
		static bool hasXPerm(const std::string &path);
		static bool hasRWPerm(const std::string &path);
		static bool hasRWXPerm(const std::string &path);

		static std::string readFile(const std::string &path);

		static std::string getMime(const std::string &filePath);
		static std::string getMime(const File &file);
		static std::string getIcon(const File &file);

		static std::string determineExtension(const std::string &fileName, size_t *extPos);
		static void		   createDiskFile(const std::string &fullPath, const std::string &body);

	public:
		class FileException : public std::exception
		{
			protected:
				std::string	   m_message;
				AHttpException m_httpException;

			public:
				FileException(const std::string &msg) throw() :
					m_message(msg),
					m_httpException(Http500Exception())
				{
				}

				virtual ~FileException() throw()
				{
				}

				virtual const char *what() const throw()
				{
					return m_message.c_str();
				}

				AHttpException getHttpException() const
				{
					return m_httpException;
				}
		};


		class FileReadException : public FileException
		{
			public:
				FileReadException(const std::string &fileName) throw() :
					FileException("Failed to read file: " + fileName)
				{
					m_httpException = Http500Exception();
				}
		};

		class FileDoesNotExistException : public FileException
		{
			public:
				FileDoesNotExistException(const std::string &fileName) throw() :
					FileException(fileName + " does not exist")
				{
					m_httpException = Http404Exception();
				}
		};

		class FilePermissionException : public FileException
		{
			public:
				FilePermissionException(const std::string &fileName) throw() :
					FileException("Permission denied for file: " + fileName)
				{
					m_httpException = Http403Exception();
				}
		};
};



#endif