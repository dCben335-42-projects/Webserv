#ifndef SEMANTIC_DESCRIPTOR_HPP
#define SEMANTIC_DESCRIPTOR_HPP

#include "Map.hpp"
#include "bnf_defs.hpp"

#include "../Rules/Group/Group.hpp"
#include "../Rules/Option/Option.hpp"
#include "../Rules/Sequence/Sequence.hpp"
#include "../Rules/Symbol/Symbol.hpp"
#include "../ConceptDef/ConceptDef.hpp"
#include "../utils/utils.hpp"
#include "../ConceptNode/ConceptNode.hpp"

#include "../Exception/ConceptAlreadyDefinedException.hpp"
#include "../Exception/InvalidConceptNameException.hpp"
#include "../Exception/ConceptHasNotDefinitionException.hpp"
#include "../Exception/EmptyConceptNameException.hpp"

class SemanticDescriptor
{
	private:
		ConceptNode		*tree;
		Vec<std::string> blocks;
		Map<ConceptNode> m_data;
		Map<ConceptList> m_concepts;
		std::string		 m_lastKey;

		bool isStartRule(const std::string &line, size_t *startDef);
		void splitBlocks(std::istringstream &iss);
		bool findSeparator(const std::string &block, size_t &pos);

		void parseConceptName(std::string);
		void parseRule(const std::string &, size_t);

		void checkValidity();
		bool hasRecursiveError(ConceptNode &node);

		void parseBlocks();
		bool hasRecursiveErrorOnNewNode(std::string item, ConceptNode *parentPtr);
		void addParent();

	public:
		SemanticDescriptor();
		SemanticDescriptor(const std::string &m_content);
		~SemanticDescriptor();

		Map<ConceptList>   getConcepts() const;
		ConceptList		  &getConcept(const std::string &key);
		const ConceptList &getConcept(const std::string &key) const;
		bool			   tryGetConcept(const std::string &key, ConceptList &conceptList) const;
		ConceptNode		  *getTree() const;
};

std::ostream &operator<<(std::ostream &os, const SemanticDescriptor &data);

#endif