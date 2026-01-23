#include <sstream>
#include <iostream>
#include "./Parser.hpp"
#include "../ARulePathInfo/ARulePathInfo.hpp"

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
	// Logger::log("OK9");
	Logger::log(items);

	path.push_back(rule);
	for (Vec<TerminalSymbol>::iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
	{
		// Logger::log("OK11");
		std::string item = itemIt->getValue();
		if (conceptPaths.contains(item))
			continue;
		if (!itemIt->isTerminalChar())
		{
			// Logger::log("OK12");
			getPathsTerminalChars(prefix, item, mapPath, conceptPaths, path);
		}
		else
		{
			Logger::log("Checking terminal char: " + item + " for concept: " + conceptName);
			Logger::log(item);
			Logger::log(ARulePath::concat(prefix, path));
			if (!mapPath.has(rule))
				mapPath.set(rule, ARulePaths());
			mapPath.get(rule).insert(ARulePathInfo(ARulePath::concat(prefix, path)));
		}
	}

	// Logger::log("OK10");

	Logger::log("Checking terminal chars for concept: " + conceptName);
	Set<const ARule *> terminalChars = m_semantic.getConcept(conceptName)->getTerminalChars();
	Logger::log("SOZA");
	Logger::log(terminalChars);
	for (Set<const ARule *>::iterator it = terminalChars.begin(); it != terminalChars.end(); ++it)
	{
		if (rule == *it)
		{
			if (!mapPath.has(rule))
				mapPath.set(rule, ARulePaths());
			mapPath.get(rule).insert(ARulePathInfo(ARulePath::concat(prefix, path)));
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
					mapPathNext.get(path[i]).insert(ARulePathInfo(path.slice(i + 1)));
				}
				else
					getPathsTerminalChars(path.slice(i + 1), terminalSymbol->getValue(), mapPathNext, Vec<std::string>(), ARulePath());
			}
		}

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
		Logger::log("---------------------------" + LoggerJson::get(conceptPaths.size()));
		while (k < size)
		{
			const ARule *subRule = (*conceptIt)->getARule(k);
			Logger::log(subRule);
			addPathPossibility(subRule, conceptPaths, path, mapPath, conceptName, prefix);
			if (!isFullyOptional(*subRule))
			{

				// Logger::log("OK20");
				break;
			}
			// Logger::log("OK28");

			++k;
		}
		Logger::log("---------------------------" + LoggerJson::get(conceptPaths.size()));
	}
}

void Parser::addNextPaths(const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext, ARulePath path, const ARule *const rule)
{
	addSequencePaths(path, mapPathNext);

	// Logger::log("OK1");


	ARulePath nextRule = rule->getNextRules(path);
	if (nextRule.empty())
		return;

	// Logger::log("OK2");

	const ARule *const lastRule = nextRule.back();
	// if (lastRule->isOption())
	// 	addNextPaths(mapPathCurrent, mapPathNext, nextRule, lastRule);
	if (isFullyOptional(*lastRule))
		addNextPaths(mapPathCurrent, mapPathNext, nextRule, lastRule);
	// Logger::log("OK3");
	// Logger::log(lastRule);

	V_FOREACH(TerminalSymbol, terminalSymbol, lastRule->getItems())
	{
		if (terminalSymbol->isTerminalChar())
		{
			// Logger::log("OK5");
			if (!mapPathNext.has(lastRule))
				mapPathNext.set(lastRule, ARulePaths());
			mapPathNext.get(lastRule).insert(ARulePathInfo(nextRule));
		}
		else
		{
			// Logger::log("OK4");
			getPathsTerminalChars(nextRule, terminalSymbol->getValue(), mapPathNext, Vec<std::string>(), ARulePath());
		}
	}
}

bool Parser::word(
	std::string::const_iterator			nextTerminalChar,
	const char							c,
	const ARule *const					rule,
	ARulePathInfo						pathInfo,
	const ARuleMap					   &mapPathCurrent,
	ARuleMap						   &mapPathNext,
	Vec<TerminalSymbol>::const_iterator item)
{
	Logger::log("---- WORD CHECK ----");
	Logger::log("Rule: " + LoggerJson::get(pathInfo));
	Logger::log("Testing character: " + LoggerJson::get(c));
	Logger::log("Next terminal char to match: " + LoggerJson::get(*nextTerminalChar));
	if (*nextTerminalChar == c)
	{
		// Logger::log("Match found for character: " + LoggerJson::get(c) + " in rule: " + rule->getConceptName());
		if (++nextTerminalChar != item->getValue().end())
		{
			ARulePathInfo pathInfoObj(pathInfo.path);
			if (!mapPathNext.has(rule))
				mapPathNext.set(rule, ARulePaths());
			pathInfoObj.terminalCharIt.push_back(nextTerminalChar);
			mapPathNext.get(rule).insert(pathInfoObj);
		}
		else
		{
			// Logger::log("Full match for character: " + LoggerJson::get(c) + " in rule: " + rule->getConceptName());
			// Logger::log(mapPathNext);

			// ARulePath nextRule = rule->getNextRules(pathInfo.path);
			// if (nextRule.back()->isOption())
			// {

			// std::string conceptName = rule->getConceptName();
			// const char *ptr			= item->getValue().c_str();
			// }

			addNextPaths(mapPathCurrent, mapPathNext, pathInfo.path, rule);
			// Logger::log(mapPathNext);
		}
		return true;
	}
	Logger::log("No match for character: " + LoggerJson::get(c) + " in rule: " + rule->getConceptName());
	return false;
}

bool Parser::parseCurrentChar(const char c, const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext)
{
	Logger::log("Char to test: " + LoggerJson::get(c));
	bool isValidWithCurrentPath = false;
	Logger::log("here01: " + LoggerJson::get(mapPathCurrent));
	for (ARuleMap::const_iterator it = mapPathCurrent.begin(); it != mapPathCurrent.end(); ++it)
	{
		const ARule		   *rule  = it->first;
		ARulePaths			paths = it->second;
		Vec<TerminalSymbol> items = rule->getItems();
		Logger::log(rule->getConceptName());
		size_t index = 0;
		Logger::log("here02: " + LoggerJson::get(paths));
		Logger::log("here02: " + LoggerJson::get(paths));
		Logger::log("here02: " + LoggerJson::get(paths));
		Logger::log("here02: " + LoggerJson::get(paths));
		Logger::log("here02: " + LoggerJson::get(paths));
		Logger::log("here02: " + LoggerJson::get(paths));
		Logger::log("here02: " + LoggerJson::get(paths));
		Logger::log("here03: " + LoggerJson::get(mapPathCurrent));
		for (Vec<TerminalSymbol>::const_iterator item = items.begin(); item != items.end(); ++item)
		{
			Logger::log("here04: " + LoggerJson::get(mapPathCurrent));
			if (item->isTerminalChar())
			{
				// Logger::log("here: " + LoggerJson::get(mapPathCurrent));
				for (ARulePaths::iterator pathInfo = paths.begin(); pathInfo != paths.end(); ++pathInfo)
				{
					Logger::log("here000002: " + LoggerJson::get(paths));

					std::string::const_iterator nextTerminalChar;
					if (!pathInfo->terminalCharIt.has(index))
					{
						Logger::log("ok5");
						nextTerminalChar = item->getValue().begin();
					}
					else
					{
						Logger::log("ok6");
						nextTerminalChar = pathInfo->terminalCharIt[index];
						Logger::log("sas: " + LoggerJson::get(*nextTerminalChar));
					}
					if (word(nextTerminalChar, c, rule, (*pathInfo), mapPathCurrent, mapPathNext, item))
						isValidWithCurrentPath = true;
				}
				index++;
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
			const ARulePath			  rules = path->path;
			const Vec<TerminalSymbol> items = rules.back()->getItems();
			for (Vec<TerminalSymbol>::const_iterator item = items.begin(); item != items.end(); ++item)
				if (item->isTerminalChar() && *(item->getValue().end() - 1) == c && isFullyOptionalEndAdvance(rules))
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
	if (stringToParsed.empty())
	{
		if (isValidEmptyFile())
			Logger::log("Parsing complete ✅");
		else
			throw FileNotFinishedException("The file must not be empty.");
		return;
	}

	ARuleMap mapPathCurrent;
	ARuleMap mapPathNext;
	char	 c;

	getPathsTerminalChars(ARulePath(), CURSOR, mapPathCurrent, Vec<std::string>(), ARulePath());

	Logger::log("Initial possible paths:");


	Logger::log(mapPathCurrent);
	for (size_t i = 0; i < stringToParsed.size(); ++i)
	{
		Logger::log("new: " + LoggerJson::get(mapPathCurrent));
		c = stringToParsed[i];
		if (!parseCurrentChar(c, mapPathCurrent, mapPathNext))
			throw InvalidEntryException(std::string("Invalid character '") + c);
		else
			Logger::log("✅");
		if (i == stringToParsed.size() - 1)
			break;

		Logger::log("after parse: " + LoggerJson::get(c));
		Logger::log("next: " + LoggerJson::get(mapPathNext));
		// std::swap(mapPathCurrent, mapPathNext);
		mapPathCurrent = mapPathNext;

		mapPathNext.clear();
	}
	if (mapPathNext.empty() || verifyOptionalEnd(mapPathCurrent, c)) // Pas tout a fait correct si il existe une suite pleinement optionnele cest aussi valide
		Logger::log("Parsing complete ✅");
	else
		throw FileNotFinishedException("END: The file ended before reaching a valid end state.");
}
