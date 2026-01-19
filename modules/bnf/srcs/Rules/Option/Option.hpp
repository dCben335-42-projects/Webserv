#ifndef OPTION_HPP
#define OPTION_HPP

#include <vector>
#include <string>
#include "bnf_defs.hpp"
#include "../ARule.hpp"

using bnf::OPTION;

class Option : public ARule
{
	public:
		Option(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr);
		~Option();
		void				 addItem(const ATerminalSymbol &val);
		Vec<ATerminalSymbol> getItems() const;
		const std::string	 getTypeName() const;
		std::string			 getPrintableValue() const;

	private:
		Vec<ATerminalSymbol> m_items;
};
#endif