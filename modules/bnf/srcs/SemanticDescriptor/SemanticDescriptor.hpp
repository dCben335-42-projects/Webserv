#ifndef SEMANTIC_DESCRIPTOR_HPP
#define SEMANTIC_DESCRIPTOR_HPP

#include "bnf_defs.hpp"

#include "../Rules/Group/Group.hpp"
#include "../Rules/Option/Option.hpp"
#include "../Rules/Sequence/Sequence.hpp"
#include "../Rules/Symbol/Symbol.hpp"
#include "../ConceptDef/ConceptDef.hpp"
#include "../utils/utils.hpp"
#include "../ConceptNode/ConceptNode.hpp"
#include "../Concept/Concept.hpp"

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
		Vec<Concept>	 m_concepts_data;
		ARuleMap		 m_startPath;
		Map<Concept *>	 m_concepts;
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
		void findAllNext();
		void ndw(ARule *rule, Vec<std::string> &conceptPaths);
		void ndw2(ARule *rule, Vec<std::string> &conceptPaths, ARulePath path, ARuleMap &mapPath, std::string conceptName) const;

	public:
		SemanticDescriptor();
		SemanticDescriptor(const std::string &m_content);
		~SemanticDescriptor();

		ARuleMap				 getStartPath() const;
		Map<Concept *>			 getConcepts() const;
		Concept				   *&getConcept(const std::string &key);
		const Concept			*getConcept(const std::string &key) const;
		const Vec<ConceptDef *> &getConceptDefs(const std::string &conceptName) const;
		ConceptNode				*getTree() const;
		ARuleMap				 getPathsTerminalChars(ARulePaths prefixs, std::string conceptName) const;
};

std::ostream &operator<<(std::ostream &os, const SemanticDescriptor &data);

#endif