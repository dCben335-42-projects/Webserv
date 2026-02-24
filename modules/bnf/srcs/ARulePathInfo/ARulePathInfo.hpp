#ifndef BNF_ARULE_PATH_INFO_HPP
#define BNF_ARULE_PATH_INFO_HPP

#include "bnf_defs.hpp"
#include "../TerminalSymbol/TerminalSymbol.hpp"

class ARule;

typedef Vec<const ARule *> ARulePath;

struct TerminalCharInfo
{
	public:
		TerminalCharInfo() : position(0), length(0)
		{
		}

		TerminalCharInfo(size_t position, size_t length) : position(position), length(length)
		{
		}
		~TerminalCharInfo()
		{
		}

		TerminalCharInfo &operator=(const TerminalCharInfo &other)
		{
			if (this == &other)
				return *this;
			position = other.position;
			length	 = other.length;
			return *this;
		}

		size_t position;
		size_t length;
};

std::ostream &operator<<(std::ostream &os, const TerminalCharInfo &data);

class ARulePathInfo
{
	public:
		ARulePath			  m_path;
		Vec<TerminalCharInfo> terminalCharsInfo;

		ARulePathInfo(const ARulePath &path);
		~ARulePathInfo();

		const Vec<std::string> &getValue() const;

		bool operator==(const ARulePathInfo &other) const;
};

std::ostream &operator<<(std::ostream &os, const ARulePathInfo &data);


typedef Vec<ARulePathInfo>					ARulePaths;
typedef Map<const ARule *const, ARulePaths> ARuleMap;

#endif