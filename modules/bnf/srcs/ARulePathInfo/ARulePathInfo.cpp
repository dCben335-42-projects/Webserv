#include "ARulePathInfo.hpp"

ARulePathInfo::ARulePathInfo(ARulePath path) : path(path), terminalCharIt(Vec<const char *>())
{
}

ARulePathInfo::~ARulePathInfo()
{
}


bool ARulePathInfo::operator==(const ARulePathInfo &other) const
{
	if (path.size() != other.path.size())
		return false;
	for (size_t i = 0; i < path.size(); ++i)
	{
		if (path[i] != other.path[i])
			return false;
	}

	if (terminalCharIt.size() != other.terminalCharIt.size())
		return false;

	for (size_t i = 0; i < terminalCharIt.size(); ++i)
	{
		if (*terminalCharIt[i] != *other.terminalCharIt[i])
			return false;
	}

	return true;
}





std::ostream &operator<<(std::ostream &os, const ARulePathInfo &data)
{
	os << "ARulePathInfo(path=[";
	for (size_t i = 0; i < data.path.size(); ++i)
	{
		os << data.path[i];
		if (i < data.path.size() - 1)
			os << ", ";
	}
	os << "], inv=[";

	for (size_t i = 0; i < data.invalidedIndexes.size(); ++i)
	{
		os << data.invalidedIndexes[i];
		if (i < data.invalidedIndexes.size() - 1)
			os << ", ";
	}
	os << "], charPtr=[";


	for (size_t i = 0; i < data.terminalCharIt.size(); ++i)
	{
		os << *(data.terminalCharIt[i]);
		if (i < data.terminalCharIt.size() - 1)
			os << ", ";
	}


	os << "])";
	return os;
}