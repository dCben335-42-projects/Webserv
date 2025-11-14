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

struct TreeNode
{
    std::string value;
    std::vector<Concept*> *concepts;
    std::vector<TreeNode*> children;
    std::vector<std::string> path;
};

class SemanticDescriptor {
    private:
        std::map<std::string, std::vector<Concept *>*> m_concepts;
        std::string m_lastKey;
        void parseConceptName(std::string& m_content);
        void parseRule(const std::string& m_content, size_t cursor);
        public:
        SemanticDescriptor() {};
        ~SemanticDescriptor() {};
        
        void createBnf(const std::string& m_content);
        void printSemanticDescriptor();
        TreeNode* createTree(TreeNode* node);
        void printTree(TreeNode* node, int depth);
        
        void checkValidity();
        void createConcept();
    
};

#endif