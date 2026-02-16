#ifndef CONFIG_LOCATION_HPP
#define CONFIG_LOCATION_HPP

#include "Config.hpp"
#include "Set.hpp"
#include "HttpMethod.hpp"
#include "LocationReturn.hpp"

class Config::Location
{
	private:
		std::string m_path;

		const Config::CgiParams *m_cgi;

		std::pair<bool, std::string>	m_root;
		std::pair<bool, std::string>	m_alias;
		std::pair<bool, std::string>	m_uploadDir;
		std::pair<bool, bool>			m_autoIndex;
		std::pair<bool, LocationReturn> m_return;

		// clang-format off
		std::pair<bool, Set<HttpMethod> >  m_allowedMethods;
		std::pair<bool, Vec<std::string> > m_indexFiles;
		// clang-format on


	public:
		Location(const std::string path);
		~Location();

		static bool compare(const Location *a, const Location *b);

		const std::string		&getPath() const;
		const std::string		&getRoot() const;
		const Vec<std::string>	&getIndexFiles() const;
		const std::string		&getAlias() const;
		bool					 getAutoIndex() const;
		const Config::CgiParams *getCgiParams() const;
		const Set<HttpMethod>	&getAllowedMethods() const;
		const std::string		&getUpload() const;
		const LocationReturn	&getReturn() const;

		bool hasAlias() const;
		bool hasCgi() const;
		bool hasIndexFile() const;
		bool hasRoot() const;
		bool hasAutoIndexSet() const;
		bool hasCgiParams() const;
		bool hasMethods() const;
		bool hasUpload() const;
		bool hasReturn() const;

		void setPath(const std::string &path);
		void setRoot(const std::string &root);
		void setAlias(const std::string &alias);
		void setIndexFile(const std::string &indexFile);
		void setAutoIndex(const bool autoIndex);
		void setCgiParams(const Config::CgiParams *cgi);
		void setMethods(const Set<HttpMethod> &methods);
		void setUpload(const std::string &upload);
		void setReturn(const LocationReturn &returnVal);

		void setIndexFiles(const Vec<std::string> &indexFiles);
		void addIndexFile(const std::string &indexFile);
};

std::ostream &operator<<(std::ostream &os, const Config::Location &location);


#endif
