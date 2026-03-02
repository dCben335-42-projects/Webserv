#ifndef BNF_PARSER_HPP
#define BNF_PARSER_HPP

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
#include "../Exception/Parser/CacheFullException.hpp"

class Parser
{
	private:
		std::map<const ARule *, bool> m_optionalCache;
		std::set<const ARule *>		  m_optionalComputing;
		Vec<const std::string *>	  m_conceptPaths;
		Vec<ARulePathInfo>			  m_arulePathInfoList;
		const SemanticDescriptor	 &m_semantic;

		bool parseCurrentChar(const char c, const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext);
		bool word(const char c, const ARule *const rule, ARulePathInfo *path, const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext, size_t indexElementTerminalChar);

		bool isFullyOptional(const ARule &rule);
		bool isFullyOptionalEndAdvance(const ARulePath &path);
		bool isFullyOptionalEnd(const ARulePath &path);

		bool isValidEmptyFile();
		bool verifyOptionalEnd(const ARuleMap &mapPathCurrent, const char c);

		void addNextPaths(const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext, const ARulePath &path, const ARule *const rule);
		void addPathPossibility(ARulePath &completePath, const ARule *const rule, ARuleMap &mapPath);
		void addSequencePaths(const ARulePath &path, ARuleMap &mapPathNext);

		void getPathsTerminalChars(ARulePath &completePath, const std::string *conceptName, ARuleMap &mapPath);
		void processTerminalSymbol(const ARule *rule, ARulePath &path, ARuleMap &mapPath);
		void createPathInfo(ARulePaths &element, const ARulePath &completePath);

		ARulePathInfo *createARulePathInfo(const ARulePath &path, size_t pathHash);
		void		   throwInvalidEntryException(const ARuleMap &mapPathCurrent, const char c, size_t i) const;

	public:
		Parser(const SemanticDescriptor &semantic);
		~Parser();

		void parse(const std::string &stringToParsed);
};

#endif