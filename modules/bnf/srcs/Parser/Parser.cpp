#include <sstream>
#include <iostream>
#include "./Parser.hpp"
#include "Vector.hpp"
#include "Map.hpp"
#include "Logger.hpp"

Parser::Parser(const SemanticDescriptor &semantic) : m_semantic(semantic)
{
}

Parser::~Parser()
{
}

// void Parser::createAllPath(std::string conceptName, PathSegment &currentPath, Vec<const PathSegment *> &allNodes)
// {
// 	ConceptList conceptList = m_semantic.getConcept(conceptName)->getDefinitions();

// 	for (size_t i = 0; i < conceptList.size(); ++i)
// 	{
// 		ConceptDef *concept = conceptList[i];
// 		Vec<ARule *> values = concept->getValues();

// 		if (concept->getIsTerminal())
// 		{
// 			for (size_t j = 0; j < values.size(); ++j)
// 			{
// 				Vec<std::string> items = values[j]->getItems();
// 				for (size_t k = 0; k < items.size(); ++k)
// 					allNodes.push(
// 						currentPath.addNextPathSegment(PathSegment(items[k], i, j, k, true)));
// 			}
// 			return;
// 		}
// 		for (size_t j = 0; j < values.size(); ++j)
// 		{
// 			Vec<std::string> items = values[j]->getItems();
// 			for (size_t k = 0; k < items.size(); ++k)
// 			{
// 				allNodes.push(
// 					currentPath.addNextPathSegment(PathSegment(items[k], i, j, k, false)));
// 				createAllPath(items[k], currentPath.getLastPathSegment(), allNodes);
// 			}
// 		}
// 	}
// }

void Parser::getAllTerminalChars(std::string conceptName, ARule *rule, Map<ARuleSet> &allTerminalChars)
{
	if (startsWith(conceptName, "\'"))
	{
		std::string key = conceptName.substr(1, 1);
		if (allTerminalChars.has(key) == false)
			allTerminalChars.set(key, Set<ARule *>());
		else
			allTerminalChars.get(key).insert(rule);
		return;
	}

	ConceptList conceptList = m_semantic.getConcept(conceptName)->getDefinitions();

	for (size_t i = 0; i < conceptList.size(); ++i)
	{
		ConceptDef *concept = conceptList[i];
		Vec<ARule *> values = concept->getValues();

		for (size_t j = 0; j < values.size(); ++j)
		{
			if (values[j]->getType() == OPTION)
			{
				Vec<std::string> items = values[j]->getItems();
				for (size_t k = 0; k < items.size(); ++k)
					getAllTerminalChars(items[k], values[j], allTerminalChars);
			}
			else if (values[j]->getType() == SYMBOL)
			{
				Vec<std::string> items = values[j]->getItems();
				getAllTerminalChars(items[0], values[j], allTerminalChars);
				break;
			}
			else
			{
				Vec<std::string> items = values[j]->getItems();
				for (size_t k = 0; k < items.size(); ++k)
					getAllTerminalChars(items[k], values[j], allTerminalChars);
				break;
			}
		}
	}
}

// ARuleSet Parser::getNext(ARule *rule, bool &hasBrother)
// {
// 	ARuleSet result;

// 	if (rule == NULL)
// 		return result;

// 	ARule *nextRule = rule->getNextRules();

// 	if (nextRule == NULL)
// 	{
// 		return m_semantic
// 			.getConcept(rule->getConceptName())
// 			->getParents();
// 	}
// 	else
// 	{
// 		result.insert(nextRule);
// 		hasBrother = true;
// 		return result;
// 	}
// }

// void Parser::recursive_parse(char c, ARuleSet &currentRuleSet, ARuleSet &nextRuleSet)
// {
// 	for (ARuleSet::iterator it = currentRuleSet.begin(); it != currentRuleSet.end(); ++it)
// 	{
// 		ARule		*rule  = *it;
// 		const ARule *rule2 = NULL;

// 		// std::string conceptName		= rule->getConceptName();
// 		// size_t		ruleIndex		= rule->getRuleIndex();
// 		// size_t		conceptDefIndex = rule->getConceptDefIndex();

// 		if (rule->getType() == SYMBOL)
// 		{
// 			bool	 hasBrother = false;
// 			ARuleSet nextRules	= getNext(rule, hasBrother);
// 			if (nextRules.empty() == true)
// 			{
// 				// Code valide si c = \n
// 			}
// 			else if (hasBrother)
// 			{
// 				for (ARuleSet::iterator nextRule = nextRules.begin(); nextRule != nextRules.end(); ++nextRule)
// 				{
// 					if ((*nextRule)->getType() == SYMBOL)
// 					{
// 						m_semantic
// 							.getConcept(rule->getConceptName())
// 							->getParents();
// 						if ((*nextRule)->getItems()[0] == std::string(1, c))
// 						{
// 						}
// 					}
// 					else
// 					{
// 					}
// 				}
// 			}
// 			for (ARuleSet::iterator nextRule = nextRules.begin(); nextRule != nextRules.end(); ++nextRule)
// 			{
// 				if ((*nextRule)->getType() == SYMBOL)
// 				{
// 				}
// 				else
// 				{
// 				}
// 			}
// 		}


// 		// {
// 		// 	ARuleSet val = m_semantic.getConcept(rule->getConceptName())->getParents();
// 		// 	if (val.empty() == true)
// 		// 	{
// 		// 		// Code valide si c = \n
// 		// 	}
// 		// 	else
// 		// 	{
// 		// 		for (ARuleSet::iterator it2 = val.begin(); it2 != val.end(); ++it2)
// 		// 		{
// 		// 			if ((*it2)->getType() == SYMBOL)
// 		// 			{
// 		// 				if ((*it2)->getItems()[0] == std::string(1, c))
// 		// 				{
// 		// 					nextRuleSet.insert(*it2);
// 		// 				}
// 		// 			}
// 		// 			else
// 		// 			{
// 		// 			}
// 		// 		}
// 		// 	}
// 		// }
// 		// else
// 		// {
// 		// 	rule2 = rule->getNextRule();
// 		// 	if (rule2->getType() == SYMBOL)
// 		// 	{
// 		// 		if (rule2->getItems()[0] == std::string(1, c))
// 		// 		{
// 		// 			nextRuleSet.insert(rule2);
// 		// 		}
// 		// 	}
// 		// }


// 		// rule2 = rule->getNextRule();
// 		// if (rule2 == NULL)
// 		// {
// 		// 	ARuleSet val = m_semantic.getConcept(rule->getConceptName())->getParents();
// 		// 	if (val.empty() == true)
// 		// 	{
// 		// 		// Code valide si c = \n
// 		// 	}
// 		// 	else
// 		// 	{
// 		// 		for (ARuleSet::iterator it2 = val.begin(); it2 != val.end(); ++it2)
// 		// 		{
// 		// 			if ((*it2)->getType() == SYMBOL)
// 		// 			{
// 		// 				if ((*it2)->getItems()[0] == std::string(1, c))
// 		// 				{
// 		// 					nextRuleSet.insert(*it2);
// 		// 				}
// 		// 			}
// 		// 			else
// 		// 			{
// 		// 			}
// 		// 		}
// 		// 	}
// 		// }
// 		// if (rule2->getType() == SYMBOL)
// 		// {
// 		// }
// 	}
// }
// (void) c;
// (void) nextRuleSet;
// }

char Parser::adaptTerminalChar(const std::string &str)
{
	if (str.length() >= 2 && (str[0] == '\'' && str[str.length() - 1] == '\''))
		return str[1];
	return '\0';
}

void Parser::searchNextRules()
{
}


// void Parser::recursive_parse(char c, ARuleSet &currentRuleSet, ARuleSet &nextRuleSet, Vec<ARule *> &path)
// {
// 	for (ARuleSet::iterator it = currentRuleSet.begin(); it != currentRuleSet.end(); ++it)
// 	{
// 		ARule			*rule  = *it;
// 		Vec<std::string> items = rule->getItems();

// 		for (size_t i = 0; i < items.size(); ++i)
// 		{
// 			if (rule->isTerminalCharAt(i))
// 			{
// 				if (adaptTerminalChar(items[i]) == c)
// 				{
// 					// rule->getNextRules();
// 				}
// 			}
// 		}
// 		// if (rule->getType() == OPTION)
// 		// {

// 		// }
// 	}
// }

void Parser::parse(const std::string &stringToParsed)
{
	// log("------------------\n");
	// if (stringToParsed.empty())
	// 	return;

	// ARuleSet	 currentARuleMap = m_semantic.getConcept(CURSOR)->getTerminalChars();
	// ARuleSet	 nextARuleMap;
	// Vec<ARule *> path;

	// for (size_t i = 0; i < stringToParsed.size(); ++i)
	// {
	// 	char c = stringToParsed[i];
	// 	recursive_parse(c, currentARuleMap, nextARuleMap, path);
	// 	currentARuleMap = nextARuleMap;
	// 	nextARuleMap.clear();
	// }
	(void) stringToParsed;
}