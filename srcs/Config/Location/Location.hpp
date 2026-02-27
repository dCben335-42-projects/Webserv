#ifndef CONFIG_LOCATION_HPP
#define CONFIG_LOCATION_HPP

#include "Set.hpp"
#include "HttpMethod.hpp"
#include "LocationReturn.hpp"
#include "CgiParams.hpp"
#include "Vec.hpp"

#include "Upload.hpp"

class Location
{
	private:
		std::string m_path;

		const CgiParams *m_cgi;

		std::pair<bool, std::string> m_root;
		std::pair<bool, std::string> m_alias;
		// std::pair<bool, std::string>	m_uploadDir;
		std::pair<bool, bool>			m_autoIndex;
		std::pair<bool, LocationReturn> m_return;
		std::pair<bool, Upload>			m_upload;
		std::pair<bool, std::string>	m_maxBodySize;

		// clang-format off
		std::pair<bool, Set<HttpMethod> >  m_allowedMethods;
		std::pair<bool, Vec<std::string> > m_indexFiles;
		// clang-format on


	public:
		Location();
		Location(const std::string path);
		Location(const Location &other);
		~Location();

		static bool compare(const Location &a, const Location &b);

		const std::string	   &getPath() const;
		const std::string	   &getRoot() const;
		const Vec<std::string> &getIndexFiles() const;
		const std::string	   &getAlias() const;
		bool					getAutoIndex() const;
		const CgiParams		   *getCgiParams() const;
		const Set<HttpMethod>  &getAllowedMethods() const;
		const Upload		   &getUpload() const;
		const LocationReturn   &getReturn() const;

		bool hasAlias() const;
		bool hasCgi() const;
		bool hasIndexFile() const;
		bool hasRoot() const;
		bool hasAutoIndex() const;
		bool hasCgiParams() const;
		bool hasMethods() const;
		bool hasReturn() const;
		bool hasIndexes() const;
		bool hasUpload() const;

		void setPath(const std::string &path);
		void setRoot(const std::string &root);
		void setAlias(const std::string &alias);
		void setAutoIndex(const bool autoIndex);
		void setUpload(const std::string &upload);
		void addIndexFile(const std::string &indexFile);
		void setCgiParams(const CgiParams *cgi);
		void setMethods(const Set<HttpMethod> &methods);
		void setMethods(const Vec<std::string> &methods);
		void setReturn(const LocationReturn &returnVal);
		void setReturn(const Vec<std::string> &returnVal);
		void setIndexes(const Vec<std::string> &indexFiles);
		void setUpload(const Vec<std::string> &upload);
};

std::ostream &operator<<(std::ostream &os, const Location &location);


#endif
