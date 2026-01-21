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

			int index	 = rules.indexOf(ruleRef);
			int ruleSize = rules.size();

			for (int k = ruleSize - 1; k >= index + 1; --k)
			{
				if (rules.back()->getType() == OPTION)
					recursiveSearchNextRules(mapPathNext, rules, rules.back());
				rules.pop_back();
			}
		}
	}
}

void Parser::recursiveSearchNextRules(ARuleMap &mapPathNext, ARulePath path, ARule *rule)
{
	ARulePath nextRule = rule->getNextRules(path);

	if (nextRule.empty())
		return;

	if (nextRule.back()->getType() == OPTION)
		recursiveSearchNextRules(mapPathNext, nextRule, nextRule.back());

	Vec<TerminalSymbol> items = nextRule.back()->getItems();

	ARulePaths prefixs;
	prefixs.insert(nextRule);
	ARuleMap mapPathCurrentTemp;

	for (size_t k = 0; k < items.size(); ++k)
	{
		ARuleMap newPaths = m_semantic.getPathsTerminalChars(prefixs, items[k].getValue());

		for (ARuleMap::iterator it = newPaths.begin(); it != newPaths.end(); ++it)
		{
			ARule	  *key = (*it).first;
			ARulePaths val = (*it).second;
			if (mapPathCurrentTemp.has(key))
				mapPathCurrentTemp.get(key).insert(val.begin(), val.end()); // A verifier si tous les chemins sont bien insere
			else
				mapPathCurrentTemp.set(key, val);
		}
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
			ARuleMap newPaths = m_semantic.getPathsTerminalChars(prefixs, items[k].getValue());
			for (ARuleMap::iterator it = newPaths.begin(); it != newPaths.end(); ++it)
			{
				ARule	  *key = (*it).first;
				ARulePaths val = (*it).second;
				if (mapPathCurrentTemp.has(key))
					mapPathCurrentTemp.get(key).insert(val.begin(), val.end()); // A verifier si tous les chemins sont bien insere
				else
					mapPathCurrentTemp.set(key, val);
			}


			// mapPathCurrentTemp.insert(newPaths.begin(), newPaths.end());
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

bool Parser::isFullyOptional(ARule *rule)
{
	if (rule->getType() == OPTION)
		return true;
	Vec<TerminalSymbol> items = rule->getItems();
	for (Vec<TerminalSymbol>::iterator item = items.begin(); item != items.end(); ++item)
	{
		if (item->isTerminalChar())
			continue;
		Vec<ConceptDef *> def = m_semantic.getConcept(item->getValue())->getDefinitions();

		for (Vec<ConceptDef *>::iterator cdef = def.begin(); cdef != def.end(); ++cdef)
		{
			ARulePath rules				= (*cdef)->getRules();
			size_t	  isOptionalCounter = 0;

			for (size_t k = 0; k < rules.size(); ++k)
				isOptionalCounter += isFullyOptional(rules[k]);
			if (isOptionalCounter == rules.size())
				return true;
		}
	}
	return false;
}

bool Parser::isFullyOptionalEnd(ARulePath path)
{
	while (path.size() > 0)
	{
		ARule *rule = path.back();
		if (!isFullyOptional(rule))
			return false;
		path = rule->getNextRules(path);
	}
	return true;
}

bool Parser::isFullyOptionalEndAdvance(ARulePath path)
{
	path = path.back()->getNextRules(path);
	return isFullyOptionalEnd(path);
}

bool Parser::verifyOptionalEnd(ARuleMap &mapPathCurrent, char c)
{
	for (ARuleMap::iterator it = mapPathCurrent.begin(); it != mapPathCurrent.end(); ++it)
	{
		ARulePaths val = it->second;

		for (ARulePaths::iterator path = val.begin(); path != val.end(); ++path)
		{
			ARulePath rules = *path;

			Vec<TerminalSymbol> items = rules.back()->getItems();

			for (Vec<TerminalSymbol>::iterator item = items.begin(); item != items.end(); ++item)
			{
				if (item->isTerminalChar())
				{
					if (*item == c)
					{
						if (isFullyOptionalEndAdvance(rules))
						{
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

void Parser::prepareOptionalStartPath(ARuleMap &mapPathCurrent, ARuleMap &mapPathNext)
{
	for (ARuleMap::iterator it = mapPathCurrent.begin(); it != mapPathCurrent.end(); ++it)
	{
		ARulePaths val = (*it).second;
		for (ARulePaths::iterator m = val.begin(); m != val.end(); ++m)
		{
			ARulePath path = *m;
			int		  size = path.size();

			for (int i = size - 1; i >= 0; --i)
			{
				if (path[i]->getType() == OPTION)
					recursiveSearchNextRules(mapPathNext, path, path.back());
				path.pop_back();
			}
		}
	}
	for (ARuleMap::iterator it = mapPathNext.begin(); it != mapPathNext.end(); ++it)
	{
		ARule	  *key = (*it).first;
		ARulePaths val = (*it).second;
		if (mapPathCurrent.has(key))
			mapPathCurrent.get(key).insert(val.begin(), val.end()); // A verifier si tous les chemins sont bien insere
		else
			mapPathCurrent.set(key, val);
	}
	mapPathNext.clear();
}

bool Parser::isValidEmptyFile()
{
	ARulePath				startPath;
	const Vec<ConceptDef *> defs = m_semantic.getConcept(CURSOR)->getDefinitions();
	for (Vec<ConceptDef *>::const_iterator defIt = defs.begin(); defIt != defs.end(); ++defIt)
	{
		startPath.clear();
		startPath.push_back((*defIt)->getARule(0));
		if (isFullyOptionalEnd(startPath))
			return true;
	}
	return false;
}

void Parser::parse(const std::string &stringToParsed)
{
	Logger::log("========Parsing========\n");
	ARuleMap mapPathCurrent = m_semantic.getStartPath();
	ARuleMap mapPathNext;
	char	 c;

	prepareOptionalStartPath(mapPathCurrent, mapPathNext);

	if (stringToParsed.empty())
	{
		if (isValidEmptyFile())
			Logger::log("Parsing complete ✅");
		else
			throw FileNotFinishedException("The file must not be empty.");
		return;
	}

	for (size_t i = 0; i < stringToParsed.size(); ++i)
	{
		c = stringToParsed[i];
		if (!parseCurrentChar(c, mapPathCurrent, mapPathNext))
			throw InvalidEntryException(std::string("Invalid character '") + c);
		else
			Logger::log("✅");
		if (i == stringToParsed.size() - 1)
			break;
		std::swap(mapPathCurrent, mapPathNext);
		mapPathNext.clear();
	}
	if (mapPathNext.empty() || verifyOptionalEnd(mapPathCurrent, c)) // Pas tout a fait correct si il existe une suite pleinement optionnele cest aussi valide
		Logger::log("Parsing complete ✅");
	else
		throw FileNotFinishedException("END: The file ended before reaching a valid end state.");
}
