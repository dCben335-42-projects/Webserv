#ifndef FILE_HPP
#define FILE_HPP

#include <string>
#include <sys/stat.h>

class File
{
	private:
		struct stat m_stat;

		std::string m_name;
		std::string m_uri;
		std::string m_fullPath;
		size_t		m_size;
		std::string m_modifiedAt;
		mode_t		m_mode;
		std::string m_content;


		std::string m_extension;
		size_t		m_extensionPos;

		bool		m_isDirectory;
		std::string m_mime;

		std::string m_sizeHuman;
		std::string m_modifiedAtHuman;
		std::string m_changedAtHuman;

		std::string m_icon;
		bool		m_isHidden;

	public:
		File(
			const std::string &name,
			const std::string &ressourcePath,
			const std::string &location,
			const std::string &defaultMime);
		File(
			const std::string &ressourcePath,
			const std::string &location,
			const std::string &defaultMime);
		~File();

		const std::string &getName() const;
		const std::string &getUri() const;
		const std::string &getFullPath() const;
		const std::string &getModifiedAt() const;

		const std::string &getMime() const;
		const std::string &getSizeHuman() const;
		const std::string &getModifiedAtHuman() const;
		const std::string &getIcon() const;

		bool getIsHidden() const;
		bool getIsDirectory() const;
		bool getHasReadPermission() const;

		size_t getExtensionPos() const;
		size_t getSize() const;

		const std::string &getExtension() const;
		const std::string &getChangedAtHuman() const;
		const std::string &getContent() const;

		File &read();
};


#endif