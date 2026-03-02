#ifndef BNF_GROUP_HPP
#define BNF_GROUP_HPP

#include <string>
#include "bnf_defs.hpp"
#include "../ARule.hpp"

using bnf::GROUP;

class Group : public ARule
{
	public:
		Group(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr, ConceptDef *conceptDefPtr);
		~Group();

		const Vec<TerminalSymbol> &getItems() const;
		const Vec<std::string>	  &getItemsValue() const;
		std::string				   getPrintableValue() const;
		const std::string		   getTypeName() const;
		void					   addItem(const TerminalSymbol &val);

	private:
		Vec<TerminalSymbol> m_items;
};
#endif