#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

#include <vector>
#include <string>
#include "bnf_defs.hpp"
#include "../ARule.hpp"

using bnf::SEQUENCE;

class Sequence : public ARule
{
	public:
		Sequence(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex);
		~Sequence();
		void			 addItem(const std::string &val);
		Vec<std::string> getItems() const;
		const std::string getTypeName() const;
		std::string		 getPrintableValue() const;

	private:
		Vec<std::string> items;
};

#endif