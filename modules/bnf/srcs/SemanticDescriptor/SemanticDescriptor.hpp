#ifndef SEMANTIC_DESCRIPTOR_HPP
#define SEMANTIC_DESCRIPTOR_HPP

#include <map>
#include <vector>
#include "bnf_defs.hpp"
#include "../Rules/ARule.hpp"
#include "../Rules/Group/Group.hpp"
#include "../Rules/Option/Option.hpp"
#include "../Rules/Sequence/Sequence.hpp"
#include "../Rules/Symbol/Symbol.hpp"
#include "../Concept/Concept.hpp"
#include "../TreeNodeData/TreeNodeData.hpp"
#include "../TreeNode/TreeNode.hpp"
#include "../utils/utils.hpp"
#include "../Exception/ConceptAlreadyDefinedException.hpp"
#include "../Exception/InvalidConceptNameException.hpp"

class SemanticDescriptor {
    private:
        Map<TreeNodeData>::type m_data;
        Map<ConceptList>::type m_concepts;
        std::string m_lastKey;
        void parseConceptName(std::string&);
        void parseRule(const std::string&, size_t);
        void checkValidity();
        bool hasRecursiveError(TreeNode* node);
    public:
        SemanticDescriptor(const std::string& m_content);
        ~SemanticDescriptor();
        Map<ConceptList>::type getConcepts() const;
};

std::ostream& operator<<(std::ostream& os, const SemanticDescriptor& data);

#endif