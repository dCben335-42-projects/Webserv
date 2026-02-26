#ifndef BNF_CONCEPT_DEF_HPP
#define BNF_CONCEPT_DEF_HPP

#include <string>
#include "bnf_defs.hpp"
#include "../../Rules/ARule.hpp"

class ConceptDef
{
	public:
		ConceptDef();
		~ConceptDef();

		const ARulePath			 &getRules() const;
		const ARule				 *getARuleUnsafe(const size_t index) const;
		const Vec<TerminalSymbol> getItems() const;
		const Vec<TerminalSymbol> getItemsValueWithOption() const;
		size_t					  size() const;
		void					  setRule(const ARule *rule);

	private:
		ARulePath m_rules;
};

std::ostream &operator<<(std::ostream &os, const ConceptDef &data);

typedef Vec<ConceptDef *> ConceptList;

#endif
