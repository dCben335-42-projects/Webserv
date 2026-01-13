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

class Parser
{
	private:
		const SemanticDescriptor &m_semantic;

		Vec<char>		 getCharAcceptedHead() const;
		Vec<char>		 getCharAccepted(ConceptNode *node) const;
		Vec<std::string> getFirstCharAccepted(ConceptNode *node) const;
		void			 detectPattern(ConceptNode *node, const char c);
		void			 recursive_parse(ConceptNode *root, const std::string &stringToParsed, size_t &charIndex);
		void			 recursive_parse(std::string conceptName, char c, std::set<char> &currentSet, std::set<char> &nextSet);
		void			 createAllPath(std::string conceptName, PathSegment &currentPath, Vec<const PathSegment *> &allNodes);
		void			 getAllTerminalChars(std::string conceptName, std::set<char> &allTerminalChars);

	public:
		~Parser();
		Parser(const SemanticDescriptor &semantic);

		void parse(const std::string &stringToParsed);
};

// std::ostream& operator<<(std::ostream& os, const SemanticDescriptor& data);

#endif