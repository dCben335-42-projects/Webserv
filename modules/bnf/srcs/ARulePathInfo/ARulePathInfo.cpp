#include "ARulePathInfo.hpp"
#include "../Rules/ARule.hpp"

ARulePathInfo::ARulePathInfo(const ARulePath &path) : m_path(path), terminalCharsInfo(Vec<TerminalCharInfo>())
{
	Vec<TerminalSymbol> items = m_path.back()->getItems();
	size_t				val	  = items.size();
	terminalCharsInfo.reserve(val);
	for (size_t i = 0; i < val; ++i)
	{
		if (items[i].isTerminalChar())
		{
			terminalCharsInfo.push_back(TerminalCharInfo(items[i], 0));
		}
	}
}

ARulePathInfo::~ARulePathInfo()
{
}


bool ARulePathInfo::operator==(const ARulePathInfo &other) const
{
	if (m_path.size() != other.m_path.size())
		return false;
	for (size_t i = 0; i < m_path.size(); ++i)
	{
		if (m_path[i] != other.m_path[i])
			return false;
	}

	if (terminalCharsInfo.size() != other.terminalCharsInfo.size())
		return false;

	for (size_t i = 0; i < terminalCharsInfo.size(); ++i)
	{
		const TerminalCharInfo &info1 = terminalCharsInfo[i];
		const TerminalCharInfo &info2 = other.terminalCharsInfo[i];

		if (&info1.terminal != &info2.terminal ||
			info1.position != info2.position ||
			info1.length != info2.length)
		{
			return false;
		}
	}
	return true;
}

std::ostream &operator<<(std::ostream &os, const ARulePathInfo &data)
{
	os << "ARulePathInfo(path=[";
	for (size_t i = 0; i < data.m_path.size(); ++i)
	{
		os << data.m_path[i];
		if (i < data.m_path.size() - 1)
			os << ", ";
	}
	os << "], charPtr=[";


	for (size_t i = 0; i < data.terminalCharsInfo.size(); ++i)
	{
		const TerminalCharInfo &info = data.terminalCharsInfo[i];
		os << "{terminal: \"" << info.terminal << "\", position: " << info.position << ", length: " << info.length << "}";
		if (i < data.terminalCharsInfo.size() - 1)
			os << ", ";
	}


	os << "])";
	return os;
}


std::ostream &operator<<(std::ostream &os, const TerminalCharInfo &data)
{
	os << "{terminal: \"" << data.terminal << "\", position: " << data.position << ", length: " << data.length << "}";
	return os;
}