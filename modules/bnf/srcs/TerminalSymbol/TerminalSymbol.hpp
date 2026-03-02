#ifndef BNF_TERMINAL_SYMBOL_HPP
#define BNF_TERMINAL_SYMBOL_HPP

#include <string>

class TerminalSymbol
{
	public:
		TerminalSymbol(const std::string &val, bool isTerminalChar);
		~TerminalSymbol();

		bool				  isTerminalChar() const;
		bool				  operator==(const char &rhs) const;
		bool				  operator==(const TerminalSymbol &rhs) const;
		const std::string	 &getValue() const;
		const char			 *getCharOfValue(size_t index) const;
		static TerminalSymbol specify(const std::string &val);
		const std::string	 *getValuePtr() const;

		TerminalSymbol &operator=(const TerminalSymbol &other);

	protected:
		std::string m_str;
		bool		m_isTerminalChar;
};

std::ostream &operator<<(std::ostream &os, const TerminalSymbol &data);

#endif