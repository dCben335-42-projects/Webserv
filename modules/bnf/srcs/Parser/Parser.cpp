#include <sstream>
#include <iostream>
#include "./Parser.hpp"

Parser::Parser(const SemanticDescriptor &semantic) : m_semantic(semantic)
{
}

Parser::~Parser()
{
}

void Parser::searchNextRules(ARuleMap &mapPathNext, ARulePaths paths, ARule *rule)
{
	for (ARulePaths::iterator path = paths.begin(); path != paths.end(); ++path)
	{
		ARulePath nextRule = rule->getNextRules(*path);
		if (nextRule.empty())
		{
			Logger::log("No next rule");
			continue;
		}
		Vec<TerminalSymbol> items = nextRule.back()->getItems();

		ARulePaths prefixs;
		prefixs.insert(nextRule);
		ARuleMap mapPathCurrentTemp;

		for (size_t k = 0; k < items.size(); ++k)
		{
			ARuleMap newPaths = m_semantic.getPathsTerminalChars(prefixs, items[k].getValue());
			mapPathCurrentTemp.insert(newPaths.begin(), newPaths.end());
		}

		for (ARuleMap::iterator it = mapPathCurrentTemp.begin(); it != mapPathCurrentTemp.end(); ++it)
		{
			ARule	  *key = (*it).first;
			ARulePaths val = (*it).second;
			for (ARulePaths::iterator m = val.begin(); m != val.end(); ++m)
			{
				if (!mapPathNext.has(key))
					mapPathNext.set(key, ARulePaths());
				mapPathNext.get(key).insert(*m);
			}
		}
	}
}

bool Parser::recursive_parse(char c, ARuleMap &mapPathCurrent, ARuleMap &mapPathNext)
{
	bool isValidWithCurrentPath = false;
	for (ARuleMap::iterator it = mapPathCurrent.begin(); it != mapPathCurrent.end(); ++it)
	{
		ARule *rule = (*it).first;
		Logger::log(rule->getConceptName());
		Vec<TerminalSymbol> items = rule->getItems();

		for (Vec<TerminalSymbol>::iterator item = items.begin(); item != items.end(); ++item)
		{
			if (item->isTerminalChar())
			{
				if (*item == c)
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

	ARuleMap mapPathCurrent = m_semantic.getStartPath();

	ARuleMap  mapPathNext;
	ARulePath path;

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