#ifndef TERMINAL_CONCEPT_HPP
#define TERMINAL_CONCEPT_HPP

#include <string>
#include "../ATerminalSymbol.hpp"

class TerminalConcept : public ATerminalSymbol
{
	public:
		TerminalConcept(const std::string &val);
		~TerminalConcept();
};

std::ostream &operator<<(std::ostream &os, const TerminalConcept &data);

#endif