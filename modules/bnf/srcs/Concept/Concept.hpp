#ifndef CONCEPT_HPP
#define CONCEPT_HPP

#include <string>
#include "bnf_defs.hpp"
#include "../ConceptDef/ConceptDef.hpp"

class Concept
{
	public:
		Concept(const std::string conceptName);
		~Concept();

		void					  pushDef(ConceptDef *definition);
		Vec<ConceptDef *>		 &getDefinitions();
		const Vec<ConceptDef *>	 &getDefinitions() const;
		const std::string		  getConceptName() const;
		void					  addParent(const ARule *parent);
		const Set<const ARule *> &getParents() const;
		void					  addTerminalChar(const ARule *terminalChar);
		const Set<const ARule *> &getTerminalChars() const;

	private:
		const std::string  m_conceptName;
		Set<const ARule *> m_terminalChars;
		Set<const ARule *> m_parents;
		Vec<ConceptDef *>  definitions;
};

std::ostream &operator<<(std::ostream &os, const Concept &data);

#endif