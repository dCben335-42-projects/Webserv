#ifndef ARULE_HPP
#define ARULE_HPP

#include "bnf_defs.hpp"
#include "Vector.hpp"
#include "Set.hpp"
#include "../TerminalSymbol/TerminalSymbol.hpp"

using bnf::ERuleType;

struct BnfMap;

class Concept;

class ARule
{
	protected:
		const ERuleType		m_type;
		const std::string	m_conceptName;
		const size_t		m_conceptDefIndex;
		const size_t		m_ruleIndex;
		Vec<bool>			m_terminalChars;
		Concept			   *m_conceptPtr;
		Set<ARule *>		m_nextRules;
		Vec<TerminalSymbol> m_items;

		ARule(ERuleType t, const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr);

	public:
		virtual ~ARule();

		ERuleType					getType() const;
		virtual const std::string	getTypeName() const				   = 0;
		virtual void				addItem(const TerminalSymbol &val) = 0;
		virtual Vec<TerminalSymbol> getItems() const				   = 0;
		virtual std::string			getPrintableValue() const		   = 0;
		static BnfMap				getMap(const std::string &opener);
		static bool					isKeyWord(const std::string &word, bool onlyClose);

		const std::string getConceptName() const;
		size_t			  getConceptDefIndex() const;
		size_t			  getRuleIndex() const;
		Vec<ARule *>	  getNextRules(Vec<ARule *> supRuleParent);
		std::string		  printNext() const;
		bool			  isTerminalCharAt(size_t index) const;
		ARule			 *tryGetAdjacentRules();

		void	 setNextRules(ARule *nextRule);
		Concept *getConceptPtr() const;
};

std::ostream &operator<<(std::ostream &os, const ARule &data);

struct BnfMap
{
		std::string opener;
		std::string closer;
		ARule *(*constructor)(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr);
		bool hasClosure;
};

#endif