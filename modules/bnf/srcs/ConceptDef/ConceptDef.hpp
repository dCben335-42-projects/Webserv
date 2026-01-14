#ifndef CONCEPT_DEF_HPP
#define CONCEPT_DEF_HPP

#include <string>
#include "bnf_defs.hpp"
#include "../Rules/ARule.hpp"
#include "Vector.hpp"
#include <set>

class ConceptDef
{
	public:
		ConceptDef(const std::string conceptName);
		~ConceptDef();

		Vec<ARule *>	 getRules();
		Vec<ARule *>	 getValues() const;
		Vec<std::string> getItems();
		Vec<std::string> getItemsWithOption();
		bool			 getIsTerminal() const;
		void			 finish();
		void			 setRule(ARule *rule);

	private:
		const std::string m_conceptName;
		Vec<ARule *>	  values;
		bool			  isTerminal;
};

std::ostream &operator<<(std::ostream &os, const ConceptDef &data);

typedef Vec<ConceptDef *> ConceptList;

#endif
