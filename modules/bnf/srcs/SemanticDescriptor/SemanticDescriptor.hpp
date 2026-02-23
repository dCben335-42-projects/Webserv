#ifndef SEMANTIC_DESCRIPTOR_HPP
#define SEMANTIC_DESCRIPTOR_HPP

#include "bnf_defs.hpp"
#include "../Rules/ARule.hpp"

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

namespace bnf
{
class SemanticDescriptor
{
	private:
		const std::string &m_fileContent;
		ConceptNode		  *tree;
		Vec<std::string>   blocks;
		Map<ConceptNode>   m_data;
		Vec<Concept>	   m_concepts_data;
		ARuleMap		   m_startPath;
		Map<Concept *>	   m_concepts;
		std::string		   m_lastKey;
		bool			   m_recursiveConceptAuthorize;

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

		bool			 parseAndConsumePrefix(std::string &str, int *out);
		Vec<std::string> splitwords(const std::string &rule);
		void			 ndw(const ARule *rule, Vec<std::string> &conceptPaths);
		void			 findAllNext();

	public:
		SemanticDescriptor(const std::string &bfnFileContent);
		~SemanticDescriptor();

		void parse();

		SemanticDescriptor &operator=(const SemanticDescriptor &other);

		ARuleMap				 getStartPath() const;
		Map<Concept *>			 getConcepts() const;
		Concept				   *&getConcept(const std::string &key);
		const Concept			*getConcept(const std::string &key) const;
		const Vec<ConceptDef *> &getConceptDefs(const std::string &conceptName) const;
		ConceptNode				*getTree() const;
};
} // namespace bnf


std::ostream &operator<<(std::ostream &os, const SemanticDescriptor &data);

#endif