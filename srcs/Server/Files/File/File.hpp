#ifndef FILE_HPP
#define FILE_HPP

#include <string>
#include <sys/stat.h>

class File
{
	private:
		std::string m_name;
		std::string m_uri;
		std::string m_fullPath;
		size_t		m_size;
		std::string m_lastModified;

		std::string m_content;

		std::string m_extension;
		size_t		m_extensionPos;

		bool		m_isDirectory;
		std::string m_contentType;

		std::string m_sizeHuman;
		std::string m_lastModifiedHuman;
		std::string m_createdAtHuman;

		std::string m_icon;
		bool		m_isHidden;

	public:
		File(
			const std::string &name,
			const std::string &ressourcePath,
			const std::string &location);
		~File();

		std::string getName() const;
		std::string getUri() const;
		std::string getFullPath() const;
		size_t		getSize() const;
		std::string getLastModified() const;

		bool		getIsDirectory() const;
		std::string getMimeType() const;
		std::string getSizeHuman() const;
		std::string getLastModifiedHuman() const;
		std::string getIcon() const;
		bool		getIsHidden() const;

		std::string getExtension() const;
		size_t		getExtensionPos() const;
		std::string getCreatedAtHuman() const;

		std::string getContent() const;

		static std::string determineExtension(const std::string &fileName, size_t *extPos);
		void			   determineIcon();
};


#endif