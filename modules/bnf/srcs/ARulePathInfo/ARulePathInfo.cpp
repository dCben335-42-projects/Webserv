#include "ARulePathInfo.hpp"
#include "../Rules/ARule.hpp"
#include "Logger.hpp"

ARulePathInfo::ARulePathInfo(const ARulePath &path) : m_path(path)
{
	const Vec<TerminalSymbol> &items = m_path.back()->getItems();

	size_t		 terminalCount = 0;
	const size_t n			   = items.size();

	for (size_t i = 0; i < n; ++i)
		if (items[i].isTerminalChar())
			++terminalCount;

	terminalCharsInfo.resize(terminalCount);

	size_t idx = 0;
	for (size_t i = 0; i < n; ++i)
	{
		const TerminalSymbol &item = items[i];

		if (item.isTerminalChar())
		{
			terminalCharsInfo[idx++] =
				TerminalCharInfo(0, item.getValue().size());
		}
	}

	m_pathHash = createPathHash(path);
}

ARulePathInfo::~ARulePathInfo()
{
}

size_t ARulePathInfo::createPathHash(const ARulePath &path)
{
	size_t pathHash = 0;
	size_t size		= path.size();
	for (size_t i = 0; i < size; ++i)
		pathHash ^= reinterpret_cast<size_t>(path[i]) + (pathHash << 5) + (pathHash >> 2);
	return pathHash;
}

const Vec<std::string> &ARulePathInfo::getValue() const
{
	return m_path.back()->getItemsValue();
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

		if (info1.position != info2.position ||
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
	// os << "], m_strs=[";
	// for (size_t i = 0; i < data.m_strs.size(); ++i)
	// {
	// 	os << data.m_strs[i];
	// 	if (i < data.m_strs.size() - 1)
	// 		os << ", ";
	// }
	os << "], charPtr=[";
	for (size_t i = 0; i < data.terminalCharsInfo.size(); ++i)
	{
		const TerminalCharInfo &info = data.terminalCharsInfo[i];
		os << "{position: " << info.position << ", length: " << info.length << "}";
		if (i < data.terminalCharsInfo.size() - 1)
			os << ", ";
	}


	os << "])";
	return os;
}


std::ostream &operator<<(std::ostream &os, const TerminalCharInfo &data)
{
	os << "{position: " << data.position << ", length: " << data.length << "}";
	return os;
}