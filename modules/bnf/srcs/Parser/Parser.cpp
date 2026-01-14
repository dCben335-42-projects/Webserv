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
			allTerminalChars.set(key, std::set<ARule *>());
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

void Parser::recursive_parse(char c, ARuleSet &currentRuleSet, ARuleSet &nextRuleSet)
{
	for (ARuleSet::iterator it = currentRuleSet.begin(); it != currentRuleSet.end(); ++it)
	{
		// ARule *rule = *it;

		// std::string conceptName		= rule->getConceptName();
		// size_t		ruleIndex		= rule->getRuleIndex();
		// size_t		conceptDefIndex = rule->getConceptDefIndex();

		// if (rule->getType() == SYMBOL)
		// {
		// 	if ()


		// 		rule++;
		// 	if (rule->getType() == SYMBOL)
		// 	{
		// 	}
		// }
	}
	(void) c;
	(void) nextRuleSet;
}
void Parser::parse(const std::string &stringToParsed)
{
	log("------------------\n");
	if (stringToParsed.empty())
		return;

	Map<ARuleSet> currentARuleMap;
	ARuleSet	  nextRuleSet;
	getAllTerminalChars(CURSOR, NULL, currentARuleMap);

	char c = stringToParsed[0];
	if (currentARuleMap.has(std::string(1, c)) == false)
	{
		std::cout << "No rule for char: " << c << std::endl;
		return;
	}
	ARuleSet ruleSet = currentARuleMap.get(std::string(1, c));

	for (size_t i = 1; i < stringToParsed.size(); ++i)
	{
		char nextChar = stringToParsed[i];
		recursive_parse(nextChar, ruleSet, nextRuleSet);
		if (nextRuleSet.empty())
		{
			std::cout << "No rule for char: " << nextChar << std::endl;
			return;
		}
		ruleSet = nextRuleSet;
	}
}