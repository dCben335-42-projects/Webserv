#ifndef SEMANTIC_DESCRIPTOR_HPP
#define SEMANTIC_DESCRIPTOR_HPP

#include "Map.hpp"
#include "bnf_defs.hpp"

#include "../Rules/Group/Group.hpp"
#include "../Rules/Option/Option.hpp"
#include "../Rules/Sequence/Sequence.hpp"
#include "../Rules/Symbol/Symbol.hpp"
#include "../Concept/Concept.hpp"

#include "../TreeNode/TreeNode.hpp"
#include "../utils/utils.hpp"
#include "../Exception/ConceptAlreadyDefinedException.hpp"
#include "../Exception/InvalidConceptNameException.hpp"
#include "../Exception/ConceptHasNotDefinitionException.hpp"
#include "../Exception/EmptyConceptNameException.hpp"

class SemanticDescriptor {
    private:
        TreeNode* tree;
        std::vector<std::string> blocks;
        Map<TreeNodeData>::type m_data;
        Map<ConceptList>::type m_concepts;
        std::string m_lastKey;
        bool isStartRule(const std::string &line, size_t *startDef);
        void splitBlocks(std::istringstream &iss);
        bool findSeparator(const std::string &block, size_t &pos);
        void parseConceptName(std::string);
        void parseRule(const std::string&, size_t);
        void checkValidity();
        bool hasRecursiveError(TreeNode* node);
        void parseBlocks();
    public:
        SemanticDescriptor(const std::string& m_content);
        ~SemanticDescriptor();
        Map<ConceptList>::type getConcepts() const;
};

std::ostream& operator<<(std::ostream& os, const SemanticDescriptor& data);

#endif