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
#include <set>

typedef std::set<ARule *> ARuleSet;

class Parser
{
	private:
		const SemanticDescriptor &m_semantic;

		void	 recursive_parse(char c, ARuleSet &currentARuleMap, ARuleSet &nextARuleMap);
		void	 createAllPath(std::string conceptName, PathSegment &currentPath, Vec<const PathSegment *> &allNodes);
		void	 getAllTerminalChars(std::string conceptName, ARule *rule, Map<ARuleSet> &allTerminalChars);
		ARuleSet getNext(ARule *rule, bool &hasBrother);

	public:
		~Parser();
		Parser(const SemanticDescriptor &semantic);

		void parse(const std::string &stringToParsed);
};

// std::ostream& operator<<(std::ostream& os, const SemanticDescriptor& data);
#endif