#ifndef BNF_SEMANTIC_DESCRIPTOR_HPP
#define BNF_SEMANTIC_DESCRIPTOR_HPP

#include "bnf_defs.hpp"
#include "../Rules/ARule.hpp"

#include "../Concept/ConceptDef/ConceptDef.hpp"
#include "../Concept/ConceptNode/ConceptNode.hpp"
#include "../utils/utils.hpp"
#include "../Concept/Concept.hpp"

#include "../Exception/SemanticDescriptor/ConceptAlreadyDefinedException.hpp"
#include "../Exception/SemanticDescriptor/InvalidConceptNameException.hpp"
#include "../Exception/SemanticDescriptor/ConceptHasNotDefinitionException.hpp"
#include "../Exception/SemanticDescriptor/EmptyConceptNameException.hpp"
#include "../Exception/SemanticDescriptor/DoubleQuoteStartException.hpp"
#include "../Exception/SemanticDescriptor/MemoryAllocationFailedException.hpp"
#include "../Exception/SemanticDescriptor/BadClosingKeywordException.hpp"
#include "../Exception/SemanticDescriptor/UnexpectedEndRuleException.hpp"
#include "../Exception/SemanticDescriptor/InvalidKeywordException.hpp"
#include "../Exception/SemanticDescriptor/ConceptNotFoundException.hpp"
#include "../Exception/SemanticDescriptor/BadPrefixException.hpp"
#include "../Exception/SemanticDescriptor/RecursiveConceptException.hpp"
#include "../Exception/SemanticDescriptor/RecursiveLoopConceptException.hpp"
#include "../Exception/SemanticDescriptor/InterruptedProgramException.hpp"

namespace bnf
{
class SemanticDescriptor
{
	private:
		ConceptNode		*m_tree;
		Vec<std::string> m_blocks;
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
		void findTerminalChar(const ARule *rule, Vec<std::string> &conceptPaths);
		void assignTerminalChar();

		bool			 parseAndConsumePrefix(std::string &str, int *out) const;
		Vec<std::string> splitwords(const std::string &rule) const;

	public:
		SemanticDescriptor();
		~SemanticDescriptor();

		void parse(const std::string &fileContent);

		const Map<Concept *>	&getConcepts() const;
		const Concept			*getConcept(const std::string &key) const;
		const Vec<ConceptDef *> &getConceptDefs(const std::string &conceptName) const;
};
} // namespace bnf


std::ostream &operator<<(std::ostream &os, const SemanticDescriptor &data);

#endif