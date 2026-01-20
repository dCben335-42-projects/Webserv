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
#include "../PathSegment/PathSegment.hpp"

typedef Set<ARule *>			 ARuleSet;
typedef Vec<ARule *>			 ARulePath;
typedef Set<ARulePath>			 ARulePaths;
typedef Map<ARule *, ARulePaths> ARuleMap;

class Parser
{
	private:
		const SemanticDescriptor &m_semantic;

		bool recursive_parse(char c, ARuleMap &mapPathCurrent, ARuleMap &mapPathNext);
		void getAllTerminalChars(std::string conceptName, ARule *rule, Map<ARuleSet> &allTerminalChars);
		char adaptTerminalChar(const std::string &str);
		void searchNextRules(ARuleMap &mapPathNext, ARulePaths paths, ARule *rule);

	public:
		~Parser();
		Parser(const SemanticDescriptor &semantic);

		void parse(const std::string &stringToParsed);
};

#endif