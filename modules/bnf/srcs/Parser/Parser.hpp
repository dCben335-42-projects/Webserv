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

		bool parseCurrentChar(char c, ARuleMap &mapPathCurrent, ARuleMap &mapPathNext);
		void searchNextRules(ARuleMap &mapPathNext, ARulePaths paths, ARule *rule);
		void recursiveSearchNextRules(ARuleMap &mapPathNext, ARulePath path, ARule *rule);
		void rec(ARuleMap &mapPath, ARuleMap &mapPathNext, ARule *ruleRef);
		bool verifyOptionalEnd(ARuleMap &mapPathCurrent, char c);
		bool detect(ARulePath rules);
		bool isFullyOptional(ARule *rule);
		bool isFullyOptionalEndAdvance(ARulePath path);
		bool isFullyOptionalEnd(ARulePath path);
		void prepareOptionalStartPath(ARuleMap &mapPathCurrent, ARuleMap &mapPathNext);
		bool isValidEmptyFile();

	public:
		~Parser();
		Parser(const SemanticDescriptor &semantic);

		void parse(const std::string &stringToParsed);
};

#endif