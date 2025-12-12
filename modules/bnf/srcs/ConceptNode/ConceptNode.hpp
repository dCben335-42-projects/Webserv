#ifndef CONCEPTNODE_HPP
#define CONCEPTNODE_HPP

#include <map>
#include <vector>
#include <iostream>
#include "bnf_defs.hpp"
#include "../Rules/ARule.hpp"
#include "../Rules/Group/Group.hpp"
#include "../Rules/Option/Option.hpp"
#include "../Rules/Sequence/Sequence.hpp"
#include "../Rules/Symbol/Symbol.hpp"
#include "../Concept/Concept.hpp"
#include "../TreeNode/TreeNode.hpp"

class ConceptNode : public TreeNode<ConceptNode>
{
	private:
		const std::string  m_name;
		const ConceptList &m_concepts;
		bool			   m_hasInfRecursivity;

	public:
		ConceptNode(const std::string name, const ConceptList &concepts);
		~ConceptNode() {};

		bool is_multiple() const;
		bool hasInfRecursivity() const;
		void setHasInfRecursivity(const bool hasInfRecursivity);

		const ConceptList  &getConcepts() const;
		const std::string  &getName() const;
		static ConceptNode *create(std::string item, Map<ConceptList> &m_concepts);
};


std::ostream &operator<<(std::ostream &os, const ConceptNode &data);

#endif