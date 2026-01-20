#ifndef OPTION_HPP
#define OPTION_HPP

#include <string>
#include "bnf_defs.hpp"
#include "../ARule.hpp"

using bnf::OPTION;

class Option : public ARule
{
	public:
		Option(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr, ConceptDef *conceptDefPtr);
		~Option();
		void				addItem(const TerminalSymbol &val);
		Vec<TerminalSymbol> getItems() const;
		const std::string	getTypeName() const;
		std::string			getPrintableValue() const;

	private:
		Vec<TerminalSymbol> m_items;
};
#endif