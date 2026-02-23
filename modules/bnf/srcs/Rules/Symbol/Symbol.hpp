#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <string>
#include "../ARule.hpp"

using bnf::SYMBOL;

class Symbol : public ARule
{
	public:
		Symbol(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr, ConceptDef *conceptDefPtr);
		Symbol(const TerminalSymbol &val, const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr, ConceptDef *conceptDefPtr);
		~Symbol();
		void					   addItem(const TerminalSymbol &val);
		const Vec<TerminalSymbol> &getItems() const;
		const std::string		  &getValue() const;
		const std::string		   getTypeName() const;
		std::string				   getPrintableValue() const;
};

std::ostream &operator<<(std::ostream &os, const Symbol &data);

#endif