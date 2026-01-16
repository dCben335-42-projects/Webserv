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

void Parser::searchNextRules(Map<ARule *, ARulePaths> &mapPathNext, ARulePaths paths, ARule *rule)
{
	for (Set<ARulePath>::iterator j = paths.begin(); j != paths.end(); ++j)
	{
		ARulePath path = *j;

		ARulePath nextRule = rule->getNextRules(path);
		if (nextRule.empty())
		{
			Logger::log("No next rule");
			continue;
		}
		ARule			*ptr   = nextRule.back();
		Vec<std::string> items = ptr->getItems();

		ARulePaths prefix = ARulePaths();
		prefix.insert(nextRule);
		Map<ARule *, ARulePaths> mapPathCurrentTemp;

		for (size_t k = 0; k < items.size(); ++k)
		{
			Map<ARule *, ARulePaths> newPaths = m_semantic.getPathsTerminalChars(prefix, items[k]);
			mapPathCurrentTemp.insert(newPaths.begin(), newPaths.end());
		}

		for (Map<ARule *, ARulePaths>::iterator it = mapPathCurrentTemp.begin(); it != mapPathCurrentTemp.end(); ++it)
		{
			ARule	  *key = (*it).first;
			ARulePaths val = (*it).second;

			for (Set<ARulePath>::iterator m = val.begin(); m != val.end(); ++m)
			{
				if (!mapPathNext.has(key))
				{
					mapPathNext.set(key, ARulePaths());
				}
				mapPathNext.get(key).insert(*m);
			}
		}
	}
}

bool Parser::recursive_parse(char c, Map<ARule *, ARulePaths> &mapPathCurrent, Map<ARule *, ARulePaths> &mapPathNext)
{
	bool isValidWithCurrentPath = false;
	for (Map<ARule *, ARulePaths>::iterator it = mapPathCurrent.begin(); it != mapPathCurrent.end(); ++it)
	{
		ARule *rule = (*it).first;
		Logger::log(rule->getConceptName());
		Vec<std::string> items = rule->getItems();

		for (size_t i = 0; i < items.size(); ++i)
		{
			if (rule->isTerminalCharAt(i))
			{
				if (adaptTerminalChar(items[i]) == c)
				{
					searchNextRules(mapPathNext, mapPathCurrent.get(rule), rule);
					isValidWithCurrentPath = true;
				}
			}
		}
	}
	return isValidWithCurrentPath;
}

void Parser::parse(const std::string &stringToParsed)
{
	Logger::log("========Parsing========\n");
	if (stringToParsed.empty())
		return;

	Map<ARule *, ARulePaths> mapPathCurrent = m_semantic.getStartPath();

	// Logger::log("Initial paths: " + LoggerJson::get(mapPathCurrent, false));
	Map<ARule *, ARulePaths> mapPathNext;
	Vec<ARule *>			 path;

	for (size_t i = 0; i < stringToParsed.size(); ++i)
	{
		char c = stringToParsed[i];
		Logger::log("Char to test: " + LoggerJson::get(c));
		bool isValidWithCurrentPath = recursive_parse(c, mapPathCurrent, mapPathNext);
		if (!isValidWithCurrentPath)
		{
			Logger::log("Pattern false ❌");
			return;
		}
		if (mapPathNext.empty() && i != stringToParsed.size() - 1)
		{
			Logger::log("Pattern false ❌");
			return;
		}
		else
		{
			Logger::log("✅");
		}
		mapPathCurrent = mapPathNext;
		mapPathNext.clear();
	}
	if (mapPathCurrent.empty()) // Pas tout a fait correct si il existe une suite pleinement optionnele cest aussi valide
	{
		Logger::log("Parsing complete ✅");
		return;
	}
	else
	{
		Logger::log("Parsing uncomplete ❌");
		return;
	}
	(void) stringToParsed;
}