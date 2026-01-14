#ifndef ARULE_HPP
#define ARULE_HPP

#include "bnf_defs.hpp"
#include "Vector.hpp"

using bnf::ERuleType;

struct BnfMap;

class ARule
{
	protected:
		const ERuleType	  m_type;
		const std::string m_conceptName;
		const size_t	  m_conceptDefIndex;
		const size_t	  m_ruleIndex;
		ARule			 *m_nextRule;

		ARule(ERuleType t, const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex);

	public:
		virtual ~ARule();

		ARule					 &operator=(const ARule &other);
		ERuleType				  getType() const;
		virtual const std::string getTypeName() const			  = 0;
		virtual void			  addItem(const std::string &val) = 0;
		virtual Vec<std::string>  getItems() const				  = 0;
		virtual std::string		  getPrintableValue() const		  = 0;
		static BnfMap			  getMap(const std::string &opener);
		static bool				  isKeyWord(const std::string &word, bool onlyClose);

		const std::string getConceptName() const;
		size_t			  getConceptDefIndex() const;
		size_t			  getRuleIndex() const;
		const ARule		 *getNextRule() const;

		void setNextRule(ARule *nextRule);
};

std::ostream &operator<<(std::ostream &os, const ARule &data);

struct BnfMap
{
		std::string opener;
		std::string closer;
		ARule *(*constructor)(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex);
		bool hasClosure;
};

#endif