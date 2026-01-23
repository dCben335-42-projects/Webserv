#include "ARulePathInfo.hpp"

ARulePathInfo::ARulePathInfo(ARulePath path) : path(path), terminalCharIt(Vec<std::string::const_iterator>())
{
}

ARulePathInfo::~ARulePathInfo()
{
}


bool ARulePathInfo::operator<(const ARulePathInfo &other) const
{
	// (void) other;
	if (path.size() != other.path.size())
		return true;
	for (size_t i = 0; i < path.size(); ++i)
	{
		if (path[i] != other.path[i])
			return true;
	}


	if (terminalCharIt.size() != other.terminalCharIt.size())
		return true;

	for (size_t i = 0; i < terminalCharIt.size(); ++i)
	{
		if (*terminalCharIt[i] != *other.terminalCharIt[i])
			return true;
	}

	return false;
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
	os << "], terminalCharIt=[";
	for (size_t i = 0; i < data.terminalCharIt.size(); ++i)
	{
		os << *(data.terminalCharIt[i]);
		if (i < data.terminalCharIt.size() - 1)
			os << ", ";
	}
	os << "])";
	return os;
}