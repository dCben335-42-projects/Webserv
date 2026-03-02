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
			terminalCharsInfo[idx++] =
				TerminalCharInfo(0, item.getValue().size());
	}

	m_pathHash	= createPathHash(path);
	replaceable = terminalCount;
}

void ARulePathInfo::resetTerminalCharsInfo()
{
	for (size_t i = 0; i < terminalCharsInfo.size(); ++i)
		terminalCharsInfo[i].position = 0;
	replaceable = terminalCharsInfo.size();
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

const ARulePath &ARulePathInfo::getPath() const
{
	return m_path;
}

std::ostream &operator<<(std::ostream &os, const ARulePathInfo &data)
{
	os << "ARulePathInfo(path=[";
	for (size_t i = 0; i < data.getPath().size(); ++i)
	{
		os << data.getPath()[i];
		if (i < data.getPath().size() - 1)
			os << ", ";
	}
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

size_t ARulePathInfo::getPathHash() const
{
	return m_pathHash;
}