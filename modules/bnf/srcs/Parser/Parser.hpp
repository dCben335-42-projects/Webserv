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
#include "../BNFPath/BNFPath.hpp"

class Parser
{
	private:
		mutable std::map<const ARule *, bool> m_optionalCache;
		mutable std::set<const ARule *>		  m_optionalComputing;

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

		void processTerminalSymbol(
			const TerminalSymbol &symbol,
			const ARule			 *rule,
			ARulePath			 &path,
			ARuleMap			 &mapPath);
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