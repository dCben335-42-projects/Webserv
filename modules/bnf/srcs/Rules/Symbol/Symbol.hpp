#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <string>
#include "../ARule.hpp"

using bnf::SYMBOL;
class Symbol : public ARule
{
	public:
		Symbol();
		Symbol(const std::string &val);
		~Symbol();
		void			   addItem(const std::string &val);
		Vec<std::string>   getItems() const;
		const std::string &getValue() const;
		std::string		   getPrintableValue() const;

	private:
		std::string value;
};

std::ostream &operator<<(std::ostream &os, const Symbol &data);

#endif