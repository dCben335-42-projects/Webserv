#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <string>
#include "../ARule.hpp"

using bnf::SYMBOL;
class Symbol : public ARule
{
	public:
		Symbol(const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex);
		Symbol(const std::string &val, const std::string &conceptName, size_t conceptDefIndex, size_t ruleIndex);
		~Symbol();
		void			   addItem(const std::string &val);
		Vec<std::string>   getItems() const;
		const std::string &getValue() const;
		const std::string  getTypeName() const;
		std::string		   getPrintableValue() const;

	private:
		std::string value;
};

std::ostream &operator<<(std::ostream &os, const Symbol &data);

#endif