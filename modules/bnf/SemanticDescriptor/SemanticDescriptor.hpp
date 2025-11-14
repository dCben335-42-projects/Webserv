#ifndef SEMANTIC_DESCRIPTOR_HPP
#define SEMANTIC_DESCRIPTOR_HPP

#include <map>
#include <vector>
#include "bnf_defs.hpp"
#include "ARule.hpp"
#include "Concept.hpp"

class SemanticDescriptor {
    private:
        std::map<std::string, std::vector<Concept *>*> m_concepts;
        std::string m_last_key;
        void parseConceptName(std::string& m_content);
        size_t parseRule(const std::string& m_content, size_t cursor, size_t concept_count);
        public:
        SemanticDescriptor() {};
        ~SemanticDescriptor() {};
        
        void createBnf(const std::string& m_content);
        void printSemanticDescriptor();
        void createConcept();
    
};

#endif