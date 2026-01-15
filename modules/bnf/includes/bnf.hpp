#ifndef BNF_HPP
#define BNF_HPP

#include "bnf_defs.hpp"
#include "../srcs/Rules/ARule.hpp"
#include "../srcs/Rules/Group/Group.hpp"
#include "../srcs/Rules/Sequence/Sequence.hpp"
#include "../srcs/Rules/Option/Option.hpp"
#include "../srcs/Rules/Symbol/Symbol.hpp"
#include "../srcs/ConceptDef/ConceptDef.hpp"
#include "../srcs/SemanticDescriptor/SemanticDescriptor.hpp"
#include "../srcs/Parser/Parser.hpp"
#include "../srcs/Concept/Concept.hpp"

namespace bnf
{
typedef ::ARule				 ARule;
typedef ::Group				 Group;
typedef ::Sequence			 Sequence;
typedef ::Option			 Option;
typedef ::Symbol			 Symbol;
typedef ::ConceptDef		 Concept;
typedef ::SemanticDescriptor SemanticDescriptor;
typedef ::Parser			 Parser;
} // namespace bnf


#endif
