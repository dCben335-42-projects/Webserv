#include "ARulePathInfo.hpp"

ARulePathInfo::ARulePathInfo(ARulePath path) : path(path), terminalCharIt(Vec<std::string::const_iterator>())
{
}

ARulePathInfo::~ARulePathInfo()
{
}


bool ARulePathInfo::operator<(const ARulePathInfo &other) const
{
	size_t minSize = (path.size() < other.path.size()) ? path.size() : other.path.size();
	for (size_t i = 0; i < minSize; ++i)
	{
		if (path[i] < other.path[i])
			return true;
		else if (path[i] > other.path[i])
			return false;
	}
	return path.size() < other.path.size();
}