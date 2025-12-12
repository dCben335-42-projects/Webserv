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
		Option();
		~Option();
		void			 addItem(const std::string &val);
		Vec<std::string> getItems() const;
		std::string		 getPrintableValue() const;

	private:
		Vec<std::string> items;
};
#endif