#ifndef CONCEPT_DEF_HPP
#define CONCEPT_DEF_HPP

#include <string>
#include "bnf_defs.hpp"
#include "../../Rules/ARule.hpp"

class ConceptDef
{
	public:
		ConceptDef(const std::string conceptName);
		~ConceptDef();

		ARulePath			getRules();
		ARulePath			getValues() const;
		Vec<TerminalSymbol> getItemsOfARule(size_t index = 0);
		Vec<TerminalSymbol> getItems();
		Vec<std::string>	getItemsValue();
		Vec<std::string>	getItemsWithOption();
		bool				getIsTerminal() const;
		void				computeOptionalIndex();
		size_t				getOptionalEndIndex() const;
		size_t				size() const;
		const ARule		   *getARule(const size_t index) const;

		void				finish();
		void				setRule(const ARule *rule);
		const std::string  &getConceptName() const;
		Vec<TerminalSymbol> getItemsValueWithOption();
		const ARulePath	   &getValuesConstRef() const;



	private:
		const std::string m_conceptName;
		ARulePath		  values;
		bool			  isTerminal;
		size_t			  optionalEndIndex;
};

std::ostream &operator<<(std::ostream &os, const ConceptDef &data);

typedef Vec<ConceptDef *> ConceptList;

#endif
