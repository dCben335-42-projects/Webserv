#ifndef GROUP_HPP
#define GROUP_HPP

#include <vector>
#include <string>
#include "bnf_defs.hpp"
#include "../ARule.hpp"

using bnf::GROUP;

class Group : public ARule
{
	public:
		Group(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex, Concept *conceptPtr);
		~Group();

		Vec<TerminalSymbol> getItems() const;
		std::string			getPrintableValue() const;
		const std::string	getTypeName() const;
		void				addItem(const TerminalSymbol &val);

	private:
		Vec<TerminalSymbol> m_items;
};
#endif