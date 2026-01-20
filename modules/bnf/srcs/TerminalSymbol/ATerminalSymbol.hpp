#ifndef ATERMINAL_SYMBOL_HPP
#define ATERMINAL_SYMBOL_HPP

#include <string>

class ATerminalSymbol
{
	public:
		virtual ~ATerminalSymbol();

		virtual bool	   isTerminalChar() const = 0;
		bool			   operator==(const char &rhs) const;
		const std::string &getTerminalSymbol() const;

	protected:
		const std::string str;
};

std::ostream &operator<<(std::ostream &os, const ATerminalSymbol &data);

#endif