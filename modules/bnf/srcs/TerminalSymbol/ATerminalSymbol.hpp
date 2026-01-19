#ifndef ATERMINAL_SYMBOL_HPP
#define ATERMINAL_SYMBOL_HPP

#include <string>

class ATerminalSymbol
{
	public:
		ATerminalSymbol(const std::string &val, bool isTerminalChar);
		~ATerminalSymbol();

		bool				   isTerminalChar() const;
		bool				   operator==(const char &rhs) const;
		bool				   operator==(const ATerminalSymbol &rhs) const;
		const std::string	  &getValue() const;
		static ATerminalSymbol specify(const std::string &val);

	protected:
		std::string m_str;
		bool		m_isTerminalChar;
};

std::ostream &operator<<(std::ostream &os, const ATerminalSymbol &data);

#endif