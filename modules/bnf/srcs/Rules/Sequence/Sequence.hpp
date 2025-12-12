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
		Sequence();
		~Sequence();
		void			 addItem(const std::string &val);
		Vec<std::string> getItems() const;
		std::string		 getPrintableValue() const;

	private:
		Vec<std::string> items;
};

#endif