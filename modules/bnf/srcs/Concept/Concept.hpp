#ifndef CONCEPT_HPP
#define CONCEPT_HPP

#include <string>
#include "bnf_defs.hpp"
#include "../Rules/ARule.hpp"
#include "Vector.hpp"

class Concept
{
	public:
		Concept();
		~Concept();

		Vec<ARule *>	 getRules();
		Vec<std::string> getItems();
		Vec<ARule *>	 getValues() const;
		void			 finish();

		void setRule(ARule *rule);

		void			 printRules() const;
		bool			 calledInConcept(std::string conceptName);
		Vec<std::string> getItemsWithOption();
		bool			 getIsTerminal() const;

	private:
		Vec<ARule *> values;
		bool		 isTerminal;
};

std::ostream &operator<<(std::ostream &os, const Concept &data);

typedef Vec<Concept *> ConceptList;

#endif
