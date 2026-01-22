#include <sstream>
#include <iostream>
#include "./Parser.hpp"

Parser::Parser(const SemanticDescriptor &semantic) : m_semantic(semantic)
{
}

Parser::~Parser()
{
}

void Parser::addPathPossibility(
	const ARule *const rule,
	Vec<std::string>  &conceptPaths,
	ARulePath		   path,
	ARuleMap		  &mapPath,
	std::string		   conceptName,
	ARulePath		   prefix) const
{
	Vec<TerminalSymbol> items = rule->getItems();

	path.push_back(rule);
	for (Vec<TerminalSymbol>::iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
	{
		std::string item = itemIt->getValue();
		if (conceptPaths.contains(item))
			continue;
		if (!itemIt->isTerminalChar())
			getPathsTerminalChars(prefix, item, mapPath, conceptPaths, path);
	}

	Set<const ARule *> terminalChars = m_semantic.getConcept(conceptName)->getTerminalChars();
	for (Set<const ARule *>::iterator it = terminalChars.begin(); it != terminalChars.end(); ++it)
	{
		if (rule == *it)
		{
			if (!mapPath.has(rule))
				mapPath.set(rule, ARulePaths());
			mapPath.get(rule).insert(ARulePath::concat(prefix, path));
		}
	}
}

void Parser::addSequencePaths(const ARulePath &path, ARuleMap &mapPathNext)
{
	for (int i = path.size() - 1; i >= 0; --i)
	{
		if (path[i]->isSequence())
		{
			V_FOREACH(TerminalSymbol, terminalSymbol, path[i]->getItems())
			{
				if (terminalSymbol->isTerminalChar())
				{
					if (!mapPathNext.has(path[i]))
						mapPathNext.set(path[i], ARulePaths());
					mapPathNext.get(path[i]).insert(path.slice(i + 1));
				}
				else
					getPathsTerminalChars(path.slice(i + 1), terminalSymbol->getValue(), mapPathNext, Vec<std::string>(), ARulePath());
			}
		}

		// Regarder si tous les noeud suivants sont optionnels
		const ARule *adjacentRule = path[i]->tryGetAdjacentRules();
		while (adjacentRule)
		{
			if (isFullyOptional(*adjacentRule))
				adjacentRule = adjacentRule->tryGetAdjacentRules();
			else
				return;
		}
	}
}

void Parser::getPathsTerminalChars(ARulePath prefix, std::string conceptName, ARuleMap &mapPath, Vec<std::string> conceptPaths, ARulePath path) const
{
	conceptPaths.push_back(conceptName);
	V_FOREACH(ConceptDef *, conceptIt, m_semantic.getConceptDefs(conceptName))
	{
		size_t size = (*conceptIt)->size();
		size_t k	= 0;
		while (k < size)
		{
			const ARule *subRule = (*conceptIt)->getARule(k);
			addPathPossibility(subRule, conceptPaths, path, mapPath, conceptName, prefix);
			if (!isFullyOptional(*subRule))
				break;
			++k;
		}
	}
}

void Parser::addNextPaths(const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext, ARulePath path, const ARule *const rule)
{
	addSequencePaths(path, mapPathNext);

	ARulePath nextRule = rule->getNextRules(path);
	if (nextRule.empty())
		return;

	const ARule *const lastRule = nextRule.back();
	if (lastRule->isOption())
		addNextPaths(mapPathCurrent, mapPathNext, nextRule, lastRule);

	V_FOREACH(TerminalSymbol, terminalSymbol, lastRule->getItems())
	getPathsTerminalChars(nextRule, terminalSymbol->getValue(), mapPathNext, Vec<std::string>(), ARulePath());
}

bool Parser::parseCurrentChar(const char c, const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext)
{
	Logger::log("Char to test: " + LoggerJson::get(c));
	bool isValidWithCurrentPath = false;
	for (ARuleMap::const_iterator it = mapPathCurrent.begin(); it != mapPathCurrent.end(); ++it)
	{
		const ARule *rule  = (*it).first;
		ARulePaths	 paths = (*it).second;
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

bool Parser::isFullyOptional(const ARule &rule) const
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
		const ARule *rule = path.back();
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
	ARuleMap mapPathCurrent;
	ARuleMap mapPathNext;
	char	 c;

	getPathsTerminalChars(ARulePath(), CURSOR, mapPathCurrent, Vec<std::string>(), ARulePath());

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
