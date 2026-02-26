#ifndef BNF_SEMANTIC_DESCRIPTOR_HPP
#define BNF_SEMANTIC_DESCRIPTOR_HPP

#include "bnf_defs.hpp"
#include "../Rules/Group/Group.hpp"
#include "../Rules/Option/Option.hpp"
#include "../Rules/Sequence/Sequence.hpp"
#include "../Rules/Symbol/Symbol.hpp"
#include "../Concept/ConceptDef/ConceptDef.hpp"
#include "../Concept/ConceptNode/ConceptNode.hpp"
#include "../utils/utils.hpp"
#include "../Concept/Concept.hpp"

#include "../Exception/ConceptAlreadyDefinedException.hpp"
#include "../Exception/InvalidConceptNameException.hpp"
#include "../Exception/ConceptHasNotDefinitionException.hpp"
#include "../Exception/EmptyConceptNameException.hpp"
#include "../Exception/DoubleQuoteStartException.hpp"
#include "../Exception/MemoryAllocationFailedException.hpp"
#include "../Exception/BadClosingKeywordException.hpp"
#include "../Exception/UnexpectedEndRuleException.hpp"
#include "../Exception/InvalidKeywordException.hpp"
#include "../Exception/ConceptNotFoundException.hpp"
#include "../Exception/BadPrefixException.hpp"
#include "../Exception/RecursiveConceptException.hpp"
#include "../Exception/RecursiveLoopConceptException.hpp"

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
		bool			 m_recursiveConceptAuthorize;

		bool isStartRule(const std::string &line, size_t *startDef);
		void splitBlocks(std::istringstream &iss);
		bool findSeparator(const std::string &block, size_t &pos);

		void parseConceptName(std::string);
		void parseRule(const std::string &, size_t);

		void checkForRecursion();
		bool hasRecursiveError(ConceptNode &node);

		void parseBlocks();
		bool hasRecursiveErrorOnNewNode(std::string item, ConceptNode *parentPtr);
		void assignParentRules();
		void clear();


		bool parseAndConsumePrefix(std::string &str, int *out);

	public:
		SemanticDescriptor(const std::string &m_content);
		~SemanticDescriptor();

		ARuleMap				 getStartPath() const;
		Map<Concept *>			 getConcepts() const;
		Concept				   *&getConcept(const std::string &key);
		const Concept			*getConcept(const std::string &key) const;
		const Vec<ConceptDef *> &getConceptDefs(const std::string &conceptName) const;
		ConceptNode				*getTree() const;
		void					 findTerminalChar(const ARule *rule, Vec<std::string> &conceptPaths);
		void					 assignTerminalChar();
		Vec<std::string>		 splitwords(std::string rule);
};

std::ostream &operator<<(std::ostream &os, const SemanticDescriptor &data);

#endif