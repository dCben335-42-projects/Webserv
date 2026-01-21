#ifndef CONFIG_LOCATION_HPP
#define CONFIG_LOCATION_HPP

#include "Config.hpp"
#include <string>

class Config::Location
{
	private:
		std::string m_path;
		std::string m_root;
		std::string m_alias;
		std::string m_indexFile;

		bool m_isAutoIndex;
		bool m_autoIndex;

		const Config::CgiParams *m_cgi;

	public:
		Location(
			const std::string		 path,
			const std::string		 root,
			const std::string		 alias,
			const std::string		 indexFile,
			const bool				 autoIndex,
			const Config::CgiParams *cgi);
		Location(
			const std::string		 path,
			const std::string		 root,
			const std::string		 alias,
			const std::string		 indexFile,
			const Config::CgiParams *cgi);
		~Location();

		const std::string		&getPath() const;
		const std::string		&getRoot() const;
		const std::string		&getIndexFile() const;
		const std::string		&getAlias() const;
		bool					 getAutoIndex() const;
		const Config::CgiParams *getCgiParams() const;

		bool hasAlias() const;
		bool hasCgi() const;
		bool hasIndexFile() const;
		bool hasRoot() const;
		bool hasAutoIndexSet() const;
		bool hasCgiParams() const;

		void setPath(const std::string &path);
		void setRoot(const std::string &root);
		void setAlias(const std::string &alias);
		void setIndexFile(const std::string &indexFile);
		void setAutoIndex(const bool autoIndex);
		void setCgiParams(const Config::CgiParams *cgi);
};

std::ostream &operator<<(std::ostream &os, const Config::Location &location);


#endif
