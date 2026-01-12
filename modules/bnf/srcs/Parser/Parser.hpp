#ifndef PARSER_HPP
#define PARSER_HPP

#include "Map.hpp"
#include "bnf_defs.hpp"

#include "../Rules/Group/Group.hpp"
#include "../Rules/Option/Option.hpp"
#include "../Rules/Sequence/Sequence.hpp"
#include "../Rules/Symbol/Symbol.hpp"
#include "../Concept/Concept.hpp"
#include "../ConceptNode/ConceptNode.hpp"
#include "../SemanticDescriptor/SemanticDescriptor.hpp"
#include "../utils/utils.hpp"
#include "../PathSegment/PathSegment.hpp"
#include <set>

class Parser
{
	private:
		const SemanticDescriptor &m_semantic;

		void			 recursive_parse(PathSegment &currentPath, char c, Map<std::string> &m_map_ref);
		Vec<std::string> getTerminalChars(ConceptNode *node);
		void			 createAllPath(std::string conceptName, PathSegment &currentPath);

	public:
		~Parser();
		Parser(const SemanticDescriptor &semantic);

		void parse(const std::string &stringToParsed);
};

// std::ostream& operator<<(std::ostream& os, const SemanticDescriptor& data);
#endif