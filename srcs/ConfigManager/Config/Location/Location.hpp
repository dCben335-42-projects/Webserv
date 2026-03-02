#ifndef CONFIG_LOCATION_HPP
#define CONFIG_LOCATION_HPP

#include "Set.hpp"
#include "HttpMethod.hpp"
#include "LocationReturn.hpp"
#include "Vec.hpp"
#include "Upload.hpp"

class Location
{
	public:
		typedef Map<std::string> CgiBin;

	private:
		std::string m_path;

		std::pair<bool, std::string> m_root;
		std::pair<bool, std::string> m_alias;

		std::pair<bool, bool>			m_autoIndex;
		std::pair<bool, LocationReturn> m_return;
		std::pair<bool, Upload>			m_upload;
		std::pair<bool, size_t>			m_maxBodySize;
		std::pair<bool, bool>			m_authRequired;

		Map<std::string>				 m_addHeaders;
		Map<unsigned short, std::string> m_errorPages;
		Map<std::string>				 m_cgiBin;

		// clang-format off
		std::pair<bool, Set<HttpMethod> >  m_methods;
		std::pair<bool, Vec<std::string> > m_indexes;
		// clang-format on


	public:
		Location();
		Location(const std::string path);
		Location(const Location &other);
		~Location();

		void reset();

		const std::string &getAlias() const;
		bool			   hasAlias() const;
		void			   setAlias(const std::string &alias);

		bool hasCgi() const;
		bool hasCgiParams() const;

		void			   setPath(const std::string &path);
		const std::string &getPath() const;

		void			   setRoot(const std::string &root);
		bool			   hasRoot() const;
		const std::string &getRoot() const;

		void setAutoIndex(const bool autoIndex);
		bool hasAutoIndex() const;
		bool getAutoIndex() const;

		void		  setUpload(const Vec<std::string> &upload);
		bool		  hasUpload() const;
		const Upload &getUpload() const;

		void					setIndexes(const Vec<std::string> &indexFiles);
		bool					hasIndexes() const;
		const Vec<std::string> &getIndexes() const;

		void				   setMethods(const Vec<std::string> &methods);
		bool				   hasMethods() const;
		const Set<HttpMethod> &getMethods() const;

		void				  setReturn(const Vec<std::string> &returnVal);
		bool				  hasReturn() const;
		const LocationReturn &getReturn() const;

		void   setMaxBodySize(const std::string &maxBodySize);
		void   setMaxBodySize(const size_t maxBodySize);
		bool   hasMaxBodySize() const;
		size_t getMaxBodySize() const;

		void setAuthRequired(const bool authRequired);
		bool hasAuthRequired() const;
		bool getAuthRequired() const;

		void					setAddHeaders(const Map<std::string> &addHeaders);
		void					combineAddHeaders(const Map<std::string> &addHeaders);
		const Map<std::string> &getAddHeaders() const;

		void									setErrorPages(const Map<std::string> &errorPages);
		void									combineErrorPages(const Map<std::string> &errorPages);
		const Map<unsigned short, std::string> &getErrorPages() const;
		bool									hasErrorPage(const unsigned short statusCode) const;
		std::string								getErrorPage(const unsigned short statusCode) const;

		void		  setCgiBin(const Map<std::string> &cgiBin);
		void		  combineCgiBin(const Map<std::string> &cgiBin);
		const CgiBin &getCgiBin() const;

		static bool compare(const Location &a, const Location &b);
};

std::ostream &operator<<(std::ostream &os, const Location &location);


#endif
