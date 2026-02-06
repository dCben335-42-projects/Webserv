#ifndef CONFIG_LOCATION_HPP
#define CONFIG_LOCATION_HPP

#include "Config.hpp"
#include "Set.hpp"
#include "HttpMethod.hpp"

class Config::Location
{
	private:
		std::string m_path;
		std::string m_root;
		std::string m_alias;
		std::string m_indexFile;
		std::string m_upload;


		const Config::CgiParams *m_cgi;

		bool			m_hasMethods;
		Set<HttpMethod> m_methods;

		bool m_hasAutoIndex;
		bool m_autoIndex;


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

		static bool compare(const Location *a, const Location *b);

		const std::string		&getPath() const;
		const std::string		&getRoot() const;
		const std::string		&getIndexFile() const;
		const std::string		&getAlias() const;
		bool					 getAutoIndex() const;
		const Config::CgiParams *getCgiParams() const;
		const Set<HttpMethod>	&getAllowedMethods() const;
		const std::string		&getUpload() const;

		bool hasAlias() const;
		bool hasCgi() const;
		bool hasIndexFile() const;
		bool hasRoot() const;
		bool hasAutoIndexSet() const;
		bool hasCgiParams() const;
		bool hasMethods() const;
		bool hasUpload() const;

		void setPath(const std::string &path);
		void setRoot(const std::string &root);
		void setAlias(const std::string &alias);
		void setIndexFile(const std::string &indexFile);
		void setAutoIndex(const bool autoIndex);
		void setCgiParams(const Config::CgiParams *cgi);
		void setMethods(const Set<HttpMethod> &methods);
		void setUpload(const std::string &upload);
};

std::ostream &operator<<(std::ostream &os, const Config::Location &location);


#endif
