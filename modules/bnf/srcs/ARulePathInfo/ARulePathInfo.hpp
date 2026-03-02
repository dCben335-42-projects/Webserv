#ifndef BNF_ARULE_PATH_INFO_HPP
#define BNF_ARULE_PATH_INFO_HPP

#include "bnf_defs.hpp"
#include "../TerminalSymbol/TerminalSymbol.hpp"

class ARule;

typedef Vec<const ARule *> ARulePath;

class TerminalCharInfo
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
	private:
		ARulePath m_path;
		size_t	  m_pathHash;

	public:
		Vec<TerminalCharInfo> terminalCharsInfo;
		unsigned int		  replaceable;

		ARulePathInfo(const ARulePath &path);
		~ARulePathInfo();

		void					resetTerminalCharsInfo();
		const Vec<std::string> &getValue() const;
		static size_t			createPathHash(const ARulePath &path);
		const ARulePath		   &getPath() const;
		size_t					getPathHash() const;
};

std::ostream &operator<<(std::ostream &os, const ARulePathInfo &data);

typedef Vec<ARulePathInfo *>				ARulePaths;
typedef Map<const ARule *const, ARulePaths> ARuleMap;

#endif