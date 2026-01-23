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

class Parser
{
	private:
		const SemanticDescriptor &m_semantic;

		bool parseCurrentChar(const char c, const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext);

		bool isFullyOptional(const ARule &rule) const;
		bool isFullyOptionalEndAdvance(const ARulePath &path);
		bool isFullyOptionalEnd(ARulePath path);

		bool isValidEmptyFile();
		bool verifyOptionalEnd(const ARuleMap &mapPathCurrent, const char c);

		void addNextPaths(const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext, ARulePath path, const ARule *const rule);
		void addPathPossibility(const ARule *const rule, Vec<std::string> &conceptPaths, ARulePath path, ARuleMap &mapPath, std::string conceptName, ARulePath prefix) const;

		void getPathsTerminalChars(ARulePath prefix, std::string conceptName, ARuleMap &mapPath, Vec<std::string> conceptPaths, ARulePath path) const;

		void addSequencePaths(const ARulePath &path, ARuleMap &mapPathNext);
		bool word(
			std::string::const_iterator			nextTerminalChar,
			const char							c,
			const ARule *const					rule,
			ARulePathInfo						pathInfo,
			const ARuleMap					   &mapPathCurrent,
			ARuleMap						   &mapPathNext,
			Vec<TerminalSymbol>::const_iterator item);

	public:
		~Parser();
		Parser(const SemanticDescriptor &semantic);

		void parse(const std::string &stringToParsed);
};

#endif