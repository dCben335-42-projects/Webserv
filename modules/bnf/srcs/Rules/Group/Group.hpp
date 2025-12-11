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
		Group();
		~Group();

		std::vector<std::string> getItems() const;
		std::string				 getPrintableValue() const;
		void					 addItem(const std::string &val);

	private:
		std::vector<std::string> items;
};
#endif