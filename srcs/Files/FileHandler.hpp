#ifndef FILEHANDLER_HPP
#define FILEHANDLER_HPP

#include <string>
#include <exception>
#include "Map.hpp"

class FileHandler
{
	private:
		FileHandler();
		~FileHandler();

		static const Map<std::string> m_extensionTypeMap;
		static Map<std::string>		  initExtensionTypeMap();

	public:
		static bool		   fileExists(const std::string &path);
		static std::string readFile(const std::string &path);
		static std::string getSafePath(
			const std::string &root,
			const std::string &requestPath);

		static std::string getContentType(const std::string &filePath);

	private:
		class FileHandlerException : public std::exception
		{
			protected:
				std::string _msg;

			public:
				FileHandlerException(const std::string &msg) throw() : _msg(msg)
				{
				}

				virtual ~FileHandlerException() throw()
				{
				}

				virtual const char *what() const throw()
				{
					return _msg.c_str();
				}
		};

		class FileReadException : public FileHandlerException
		{
			public:
				FileReadException(const std::string &fileName) throw() :
					FileHandlerException("Failed to read file: " + fileName)
				{
				}
		};

		class FileDoesNotExistException : public FileHandlerException
		{
			public:
				FileDoesNotExistException(const std::string &fileName) throw() :
					FileHandlerException(fileName + " does not exist")
				{
				}
		};

		class InvalidPathException : public FileHandlerException
		{
			public:
				InvalidPathException(const std::string &path, const std::string &file) throw() :
					FileHandlerException("Invalid path: " + path + " for file: " + file)
				{
				}
		};
};

#endif