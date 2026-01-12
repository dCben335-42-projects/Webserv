#ifndef FILEHANDLER_HPP
#define FILEHANDLER_HPP

#include <string>
#include <exception>
#include "Map.hpp"
#include "httpExceptions.hpp"

class FileHandler
{
	private:
		FileHandler();
		~FileHandler();

		static const Map<std::string> m_extensionTypeMap;
		static Map<std::string>		  initExtensionTypeMap();

	public:
		static bool fileExists(const std::string &path);
		static bool hasReadPermission(const std::string &path);

		static std::string readFile(const std::string &path);
		static std::string getSafePath(
			const std::string &root,
			const std::string &requestPath);

		static std::string getContentType(const std::string &filePath);
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

	private:
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