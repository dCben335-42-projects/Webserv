#ifndef FILEHANDLER_HPP
#define FILEHANDLER_HPP

#include <string>
#include <exception>
#include "Map.hpp"
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


class FileHandler
{
	private:
		FileHandler();
		~FileHandler();

		static const Map<std::string> m_extensionTypeMap;
		static Map<std::string>		  initExtensionTypeMap();

		static const Map<std::string> m_iconMap;
		static Map<std::string>		  initIconMap();

	public:
		static bool fileExists(const std::string &path);
		static bool folderExists(const std::string &path);
		static bool hasReadPermission(const std::string &path);

		static std::string readFile(const std::string &path);
		static std::string getSafePath(
			const std::string &root,
			const std::string &requestPath);

		static std::string getContentType(const std::string &filePath);
		static std::string getContentType(const File &file);
		static std::string getIcon(const File &file);

		static std::string findLastModifiedDate(const std::string &filePath);


	public:
		class FileHandlerException : public std::exception
		{
			protected:
				std::string	   m_message;
				AHttpException m_httpException;

			public:
				FileHandlerException(const std::string &msg) throw() :
					m_message(msg),
					m_httpException(Http500Exception())
				{
				}

				virtual ~FileHandlerException() throw()
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


		class FileReadException : public FileHandlerException
		{
			public:
				FileReadException(const std::string &fileName) throw() :
					FileHandlerException("Failed to read file: " + fileName)
				{
					m_httpException = Http500Exception();
				}
		};

		class FileDoesNotExistException : public FileHandlerException
		{
			public:
				FileDoesNotExistException(const std::string &fileName) throw() :
					FileHandlerException(fileName + " does not exist")
				{
					m_httpException = Http404Exception();
				}
		};

		class FilePermissionException : public FileHandlerException
		{
			public:
				FilePermissionException(const std::string &fileName) throw() :
					FileHandlerException("Permission denied for file: " + fileName)
				{
					m_httpException = Http403Exception();
				}
		};

		class InvalidPathException : public FileHandlerException
		{
			public:
				InvalidPathException(const std::string &path, const std::string &file) throw() :
					FileHandlerException("Invalid path: " + path + " for file: " + file)
				{
					m_httpException = Http404Exception();
				}
		};
};



#endif