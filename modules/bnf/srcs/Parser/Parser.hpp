#ifndef PARSER_HPP
#define PARSER_HPP

#include "Map.hpp"
#include "bnf_defs.hpp"

#include "../Rules/ARule.hpp"
#include "../Rules/Group/Group.hpp"
#include "../Rules/Option/Option.hpp"
#include "../Rules/Sequence/Sequence.hpp"
#include "../Rules/Symbol/Symbol.hpp"
#include "../Concept/ConceptDef/ConceptDef.hpp"
#include "../Concept/ConceptNode/ConceptNode.hpp"
#include "../SemanticDescriptor/SemanticDescriptor.hpp"
#include "../utils/utils.hpp"

#include "../Exception/Parser/InvalidEntryException.hpp"
#include "../Exception/Parser/FileNotFinishedException.hpp"
#include "../BNFPath/BNFPath.hpp"

// typedef Vec<ARulePath> ARulePathsOk;

// class PathCacheKey
// {
// 	public:
// 		std::string conceptName;
// 		ARulePath	pathContext;

// 		PathCacheKey();
// 		PathCacheKey(const std::string &name, const ARulePath &path);

// 		bool operator==(const PathCacheKey &other) const;

// 		bool operator<(const PathCacheKey &other) const;
// };

// std::ostream &operator<<(std::ostream &os, const PathCacheKey &key);

// subPath, terminalSymbol->getValue()

class Parser
{
	private:
		// Set<PathCacheKey>					  m_computing;
		mutable std::map<const ARule *, bool> m_optionalCache;
		mutable std::set<const ARule *>		  m_optionalComputing;
		// std::map<std::pair<const ARule *, std::string>, ARulePaths> m_pathsCache;
		// std::map<std::string, ARuleMap> m_pathsCache;
		// Map<PathCacheKey, ARuleMap> m_pathsCache;
		const SemanticDescriptor &m_semantic;

		bool parseCurrentChar(const char c, const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext);

		bool isFullyOptional(const ARule &rule) const;
		bool isFullyOptionalEndAdvance(const ARulePath &path);
		bool isFullyOptionalEnd(ARulePath path);

		bool isValidEmptyFile();
		bool verifyOptionalEnd(const ARuleMap &mapPathCurrent, const char c);

		void addNextPaths(const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext, const ARulePath &path, const ARule *const rule);
		void addPathPossibility(const ARule *const rule, Vec<std::string> &conceptPaths, ARuleMap &mapPath, const std::string &conceptName, ARulePath &completePath);
		void addSequencePaths(const ARulePath &path, ARuleMap &mapPathNext);

		void getPathsTerminalChars(ARulePath &completePath, const std::string &conceptName, ARuleMap &mapPath, Vec<std::string> &conceptPaths);
		void precomputeOptional(const ARule *rule);

		bool word(
			TerminalCharInfo  &data,
			const char		   c,
			const ARule *const rule,
			const ARulePath	  &path,
			const ARuleMap	  &mapPathCurrent,
			ARuleMap		  &mapPathNext);

	public:
		Parser(const SemanticDescriptor &semantic);
		~Parser();

		void parse(const std::string &stringToParsed);
};

#endif