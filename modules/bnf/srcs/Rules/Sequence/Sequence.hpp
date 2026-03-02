#ifndef BNF_SEQUENCE_HPP
#define BNF_SEQUENCE_HPP

#include <string>
#include "bnf_defs.hpp"
#include "../ARule.hpp"

using bnf::SEQUENCE;

class Sequence : public ARule
{
	public:
		Sequence(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr, ConceptDef *conceptDefPtr);
		~Sequence();
		void					   addItem(const TerminalSymbol &val);
		const Vec<TerminalSymbol> &getItems() const;
		const Vec<std::string>	  &getItemsValue() const;
		const std::string		   getTypeName() const;
		std::string				   getPrintableValue() const;
};

#endif