#ifndef BNF_DEFS_HPP
#define BNF_DEFS_HPP

#include <string>

namespace bnf
{
enum ERuleType
{
	SYMBOL,
	GROUP,
	SEQUENCE,
	OPTION
};

static const std::string SEMI						 = ";";
static const std::string LBRACKET					 = "{";
static const std::string RBRACKET					 = "}";
static const std::string LBRACE						 = "(";
static const std::string RBRACE						 = ")";
static const std::string LHOOK						 = "[";
static const std::string RHOOK						 = "]";
static const std::string DQSQ						 = "'";
static const std::string DIVIDER					 = "|";
static const std::string EQRULE						 = "::=";
static const std::string ORRULE						 = "|=";
static const std::string WS							 = " \t\n\r";
static const std::string CURSOR						 = "<head>";
static const std::string ERR_INVALID_CONCEPT_NAME	 = "\033[33mError: Concept name invalid\033[0m";
static const std::string ERR_CONCEPT_ALREADY_DEFINED = "\033[33mError: Concept name already defined\033[0m ";
static const std::string ERR_CONCEPT_NOT_FOUND		 = "\033[33mError: Concept not found\033[0m ";
static const std::string ERR_CONCEPT_NOT_DEFINED	 = "\033[33mError: Concept not defined\033[0m";
} // namespace bnf

#define V_FOREACH(TYPE, op, container) \
	Vec<TYPE> vec = (container); \
	for (Vec<TYPE>::const_iterator op = vec.begin(); op != vec.end(); ++op) 


#endif
