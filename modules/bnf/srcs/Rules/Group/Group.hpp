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

		Vec<std::string> getItems() const;
		std::string		 getPrintableValue() const;
		void			 addItem(const std::string &val);

	private:
		Vec<std::string> items;
};
#endif