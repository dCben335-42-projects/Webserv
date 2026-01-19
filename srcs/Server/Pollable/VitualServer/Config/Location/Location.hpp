#ifndef CONFIG_LOCATION_HPP
#define CONFIG_LOCATION_HPP

#include "Config.hpp"
#include <string>

class Config::Location
{
	private:
		std::string		   m_path;
		std::string		   m_root;
		std::string		   m_indexFile;
		bool			   m_autoIndex;
		Config::CgiParams *m_cgi;


	public:
		Location();
		Location(
			std::string		   path,
			std::string		   root,
			std::string		   indexFile,
			bool			   autoIndex,
			Config::CgiParams *cgi);
		~Location();

		const std::string		&getPath() const;
		const std::string		&getRoot() const;
		const std::string		&getIndexFile() const;
		bool					 getAutoIndex() const;
		const Config::CgiParams *getCgiParams() const;

		void setPath(const std::string &path);
		void setRoot(const std::string &root);
		void setIndexFile(const std::string &indexFile);
		void setAutoIndex(bool autoIndex);
		void setCgiParams(Config::CgiParams *cgi);
};

#endif
