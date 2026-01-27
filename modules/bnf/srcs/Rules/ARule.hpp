#ifndef ARULE_HPP
#define ARULE_HPP

#include "bnf_defs.hpp"
#include "../TerminalSymbol/TerminalSymbol.hpp"
#include "../ARulePathInfo/ARulePathInfo.hpp"

using bnf::ERuleType;

class Concept;
class ConceptDef;

struct BnfMap
{
		std::string opener;
		std::string closer;
		ARule *(*constructor)(
			const std::string &conceptName,
			size_t			   conceptDefIndex,
			size_t			   ruleIndex,
			Concept			  *conceptPtr,
			ConceptDef		  *conceptDefPtr);
		bool hasClosure;
};

class ARule
{
	protected:
		const ERuleType		m_type;
		const std::string	m_conceptName;
		const size_t		m_conceptDefIndex;
		const size_t		m_ruleIndex;
		Concept			   *m_conceptPtr;
		ConceptDef		   *m_conceptDefPtr;
		Set<const ARule *>	m_nextRules;
		Vec<TerminalSymbol> m_items;

		ARule(ERuleType t, const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr, ConceptDef *conceptDefPtr);

	public:
		virtual ~ARule();

		ERuleType						   getType() const;
		virtual const std::string		   getTypeName() const				  = 0;
		virtual void					   addItem(const TerminalSymbol &val) = 0;
		virtual const Vec<TerminalSymbol> &getItems() const					  = 0;
		virtual std::string				   getPrintableValue() const		  = 0;
		static BnfMap					   getMap(const std::string &opener);
		static bool						   isKeyWord(const std::string &word, bool onlyClose);

		const std::string getConceptName() const;
		size_t			  getConceptDefIndex() const;
		size_t			  getRuleIndex() const;
		ARulePath		  getNextRules(ARulePath supRuleParent) const;


		std::string	 printNext() const;
		const ARule *tryGetAdjacentRules() const;
		ConceptDef	*getConceptDefPtr();


		bool isOption() const;
		bool isSequence() const;
		bool is(const ERuleType type) const;

		void	 setNextRules(const ARule *nextRule);
		Concept *getConceptPtr() const;
};





std::ostream &operator<<(std::ostream &os, const ARule &data);

#endif