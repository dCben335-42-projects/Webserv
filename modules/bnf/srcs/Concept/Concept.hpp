#ifndef CONCEPT_HPP
#define CONCEPT_HPP

#include <string>
#include "bnf_defs.hpp"
#include "../ConceptDef/ConceptDef.hpp"
#include "Vector.hpp"
#include <set>

class Concept
{
	public:
		Concept(const std::string conceptName);
		~Concept();

		void					 pushDef(ConceptDef *definition);
		Vec<ConceptDef *>		&getDefinitions();
		const Vec<ConceptDef *> &getDefinitions() const;
		const std::string		 getConceptName() const;
		void					 addParent(ARule *parent);
		const std::set<ARule *> &getParents() const;

	private:
		const std::string m_conceptName;
		std::set<ARule *> m_parents;
		Vec<ConceptDef *> definitions;
};

std::ostream &operator<<(std::ostream &os, const Concept &data);

#endif