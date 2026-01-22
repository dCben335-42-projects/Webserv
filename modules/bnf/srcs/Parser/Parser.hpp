#ifndef PARSER_HPP
#define PARSER_HPP

#include "Map.hpp"
#include "bnf_defs.hpp"

#include "../Rules/Group/Group.hpp"
#include "../Rules/Option/Option.hpp"
#include "../Rules/Sequence/Sequence.hpp"
#include "../Rules/Symbol/Symbol.hpp"
#include "../ConceptDef/ConceptDef.hpp"
#include "../ConceptNode/ConceptNode.hpp"
#include "../SemanticDescriptor/SemanticDescriptor.hpp"
#include "../utils/utils.hpp"

#include "../Exception/Parser/InvalidEntryException.hpp"
#include "../Exception/Parser/FileNotFinishedException.hpp"

class Parser
{
	private:
		const SemanticDescriptor &m_semantic;

		bool   parseCurrentChar(const char c, const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext);
		void   addNextPaths(const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext, ARulePath path, ARule *rule);
		void   addOptionalPaths(const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext, ARule *ruleRef);
		bool   verifyOptionalEnd(const ARuleMap &mapPathCurrent, const char c);
		bool   isFullyOptional(const ARule &rule);
		bool   isFullyOptionalEndAdvance(const ARulePath &path);
		bool   isFullyOptionalEnd(ARulePath path);
		void   prepareOptionalStartPath(ARuleMap &mapPathCurrent, ARuleMap &mapPathNext);
		bool   isValidEmptyFile();
		size_t addSequencePaths(const ARulePath &path, ARuleMap &mapPathNext, ARuleMap &mapPathCurrent, ARule *rule);
		void   getPathsTerminalChars(ARulePath prefix, std::string conceptName, ARuleMap &mapPath) const;
		void   getNewMapPath(ARulePath prefix, Vec<TerminalSymbol> terminalSymbols, ARuleMap &mapPathNext) const;
		void   addPathPossibility(ARule *rule, Vec<std::string> &conceptPaths, ARulePath path, ARuleMap &mapPath, std::string conceptName, ARulePath prefix) const;

	public:
		~Parser();
		Parser(const SemanticDescriptor &semantic);

		void parse(const std::string &stringToParsed);
};

#endif