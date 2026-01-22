#include <sstream>
#include <iostream>
#include "./Parser.hpp"

Parser::Parser(const SemanticDescriptor &semantic) : m_semantic(semantic)
{
}

Parser::~Parser()
{
}

void Parser::addOptionalPaths(const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext, ARule *ruleRef)
{
	ARule *currentRule;
	for (ARuleMap::iterator it = mapPathNext.begin(); it != mapPathNext.end(); ++it)
	{
		ARulePaths val = it->second;
		for (ARulePaths::iterator path = val.begin(); path != val.end(); ++path)
		{
			ARulePath rules = *path;
			int		  index = rules.indexOf(ruleRef);
			if (index == -1)
				continue;

			for (int k = rules.size() - 1; k >= index + 1; --k)
			{
				currentRule = rules.back();
				if (currentRule->isOption())
					addNextPaths(mapPathCurrent, mapPathNext, rules, currentRule);
				rules.pop_back();
			}
		}
	}
}

void Parser::addNextPaths(const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext, ARulePath path, ARule *rule)
{
	if (rule->isSequence())
	{
		const ARulePaths &paths = mapPathCurrent.get(rule);
		if (!mapPathNext.has(rule))
			mapPathNext.set(rule, ARulePaths());
		for (ARulePaths::iterator path = paths.begin(); path != paths.end(); ++path)
			mapPathNext.get(rule).insert(*path);
	}
	ARulePath nextRule = rule->getNextRules(path);
	if (nextRule.empty())
		return;

	ARule *lastRule = nextRule.back();
	if (lastRule->isOption())
		addNextPaths(mapPathCurrent, mapPathNext, nextRule, lastRule);

	m_semantic.getNewMapPath(nextRule, lastRule->getItems(), mapPathNext);
	addOptionalPaths(mapPathCurrent, mapPathNext, lastRule);
}

bool Parser::parseCurrentChar(const char c, const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext)
{
	Logger::log("Char to test: " + LoggerJson::get(c));
	bool isValidWithCurrentPath = false;
	for (ARuleMap::const_iterator it = mapPathCurrent.begin(); it != mapPathCurrent.end(); ++it)
	{
		ARule	  *rule	 = (*it).first;
		ARulePaths paths = (*it).second;
		Logger::log(rule->getConceptName());
		Vec<TerminalSymbol> items = rule->getItems();
		for (Vec<TerminalSymbol>::const_iterator item = items.begin(); item != items.end(); ++item)
		{
			if (item->isTerminalChar() && *item == c)
			{
				for (ARulePaths::iterator path = paths.begin(); path != paths.end(); ++path)
					addNextPaths(mapPathCurrent, mapPathNext, *path, rule);
				isValidWithCurrentPath = true;
			}
		}
	}
	return isValidWithCurrentPath;
}

bool Parser::isFullyOptional(const ARule &rule)
{
	if (rule.isOption())
		return true;
	Vec<TerminalSymbol> items = rule.getItems();
	for (Vec<TerminalSymbol>::const_iterator item = items.begin(); item != items.end(); ++item)
	{
		if (item->isTerminalChar())
			continue;
		const Vec<ConceptDef *> &def = m_semantic.getConceptDefs(item->getValue());
		for (Vec<ConceptDef *>::const_iterator cdef = def.begin(); cdef != def.end(); ++cdef)
		{
			ARulePath rules				= (*cdef)->getRules();
			size_t	  isOptionalCounter = 0;

			for (size_t k = 0; k < rules.size(); ++k)
				isOptionalCounter += isFullyOptional(*(rules[k]));
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
		if (!isFullyOptional(*rule))
			return false;
		path = rule->getNextRules(path);
	}
	return true;
}

bool Parser::isFullyOptionalEndAdvance(const ARulePath &path)
{
	return isFullyOptionalEnd(path.back()->getNextRules(path));
}

bool Parser::verifyOptionalEnd(const ARuleMap &mapPathCurrent, const char c)
{
	for (ARuleMap::const_iterator it = mapPathCurrent.begin(); it != mapPathCurrent.end(); ++it)
	{
		const ARulePaths val = it->second;
		for (ARulePaths::const_iterator path = val.begin(); path != val.end(); ++path)
		{
			const ARulePath			  rules = *path;
			const Vec<TerminalSymbol> items = rules.back()->getItems();
			for (Vec<TerminalSymbol>::const_iterator item = items.begin(); item != items.end(); ++item)
				if (item->isTerminalChar() && *item == c && isFullyOptionalEndAdvance(rules))
					return true;
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
			for (int i = path.size() - 1; i >= 0; --i)
			{
				if (path[i]->isOption())
					addNextPaths(mapPathCurrent, mapPathNext, path, path.back());
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
	ARulePath				 startPath;
	const Vec<ConceptDef *> &defs = m_semantic.getConceptDefs(CURSOR);
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
