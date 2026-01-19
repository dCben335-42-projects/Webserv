#include <sstream>
#include <iostream>
#include "./Parser.hpp"

Parser::Parser(const SemanticDescriptor &semantic) : m_semantic(semantic)
{
}

Parser::~Parser()
{
}

// on doit regarder si rule est optional ici si optional il faut faire le chemin vers leleement suivant aussi

void Parser::rec(ARuleMap &mapPath, ARuleMap &mapPathNext, ARule *ruleRef)
{
	for (ARuleMap::iterator it = mapPath.begin(); it != mapPath.end(); ++it)
	{
		ARulePaths val = (*it).second;
		for (ARulePaths::iterator path = val.begin(); path != val.end(); ++path)
		{
			ARulePath rules = *path;

			size_t index = rules.indexOf(ruleRef);

			for (size_t k = index + 1; k < rules.size(); ++k)
			{
				ARule *rule = rules[k];
				if (rule->getType() == OPTION)
					recursiveSearchNextRules(mapPathNext, rules, rule);
			}
		}
	}
}


void Parser::recursiveSearchNextRules(ARuleMap &mapPathNext, ARulePath path, ARule *rule)
{
	ARulePath nextRule = rule->getNextRules(path);

	if (nextRule.back()->getType() == OPTION)
		recursiveSearchNextRules(mapPathNext, nextRule, nextRule.back());

	Vec<TerminalSymbol> items = nextRule.back()->getItems();

	ARulePaths prefixs;
	prefixs.insert(nextRule);
	ARuleMap mapPathCurrentTemp;

	for (size_t k = 0; k < items.size(); ++k)
	{
		ARuleMap newPaths = m_semantic.getPathsTerminalChars(prefixs, items[k].getValue());
		mapPathCurrentTemp.insert(newPaths.begin(), newPaths.end());
	}
	rec(mapPathCurrentTemp, mapPathNext, nextRule.back());

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


void Parser::searchNextRules(ARuleMap &mapPathNext, ARulePaths paths, ARule *rule)
{
	for (ARulePaths::iterator path = paths.begin(); path != paths.end(); ++path)
	{
		ARulePath nextRule = rule->getNextRules(*path);
		if (nextRule.empty())
			continue;

		if (nextRule.back()->getType() == OPTION)
			recursiveSearchNextRules(mapPathNext, nextRule, nextRule.back());

		Vec<TerminalSymbol> items = nextRule.back()->getItems();

		ARulePaths prefixs;
		prefixs.insert(nextRule);
		ARuleMap mapPathCurrentTemp;

		for (size_t k = 0; k < items.size(); ++k)
		{
			ARuleMap newPaths = m_semantic.getPathsTerminalChars(prefixs, items[k].getValue()); // DOIT FAIRE RECURSIVITE OPTIONAL SUR CHAQUE LEVEL
			mapPathCurrentTemp.insert(newPaths.begin(), newPaths.end());
		}
		rec(mapPathCurrentTemp, mapPathNext, nextRule.back());

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

bool Parser::parseCurrentChar(char c, ARuleMap &mapPathCurrent, ARuleMap &mapPathNext)
{
	Logger::log("Char to test: " + LoggerJson::get(c));
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
		throw FileNotFinishedException("START: The file ended before reaching a valid end state.");
	ARuleMap mapPathCurrent = m_semantic.getStartPath();
	ARuleMap mapPathNext;

	for (size_t i = 0; i < stringToParsed.size(); ++i)
	{
		char c = stringToParsed[i];
		if (!parseCurrentChar(c, mapPathCurrent, mapPathNext))
			throw InvalidEntryException(std::string("Invalid character '") + c);
		else
			Logger::log("✅");
		std::swap(mapPathCurrent, mapPathNext);
		mapPathNext.clear();
	}
	if (mapPathCurrent.empty()) // Pas tout a fait correct si il existe une suite pleinement optionnele cest aussi valide
		Logger::log("Parsing complete ✅");
	else
		throw FileNotFinishedException("END: The file ended before reaching a valid end state.");
}