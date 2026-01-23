#ifndef ARULE_PATH_INFO_HPP
#define ARULE_PATH_INFO_HPP

#include "bnf_defs.hpp"

class ARule;

typedef Vec<const ARule *> ARulePath;

class ARulePathInfo
{
	public:
		ARulePath						 path;
		Vec<std::string::const_iterator> terminalCharIt;
		// char							*ptr;

		ARulePathInfo(ARulePath path);
		~ARulePathInfo();

		bool operator<(const ARulePathInfo &other) const;
};

std::ostream &operator<<(std::ostream &os, const ARulePathInfo &data);


typedef Set<ARulePathInfo>					ARulePaths;
typedef Map<const ARule *const, ARulePaths> ARuleMap;

#endif