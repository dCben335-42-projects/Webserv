#include <sstream>
#include <iostream>
#include <algorithm>
#include "../ARulePathInfo/ARulePathInfo.hpp"
#include "../BNFPath/BNFPath.hpp"
#include "Parser.hpp"


namespace bnf
{
Parser::Parser(const SemanticDescriptor &semantic) : m_semantic(semantic)
{
}

Parser::~Parser()
{
}

void Parser::addPathPossibility(
	const ARule *const rule,
	Vec<std::string>  &conceptPaths,
	ARuleMap		  &mapPath,
	const std::string &conceptName,
	ARulePath		  &completePath)
{
	const Vec<TerminalSymbol> &items = rule->getItems();
	const size_t			   size	 = items.size();
	completePath.push_back(rule);

	for (size_t i = 0; i < size; ++i)
	{
		const TerminalSymbol &item		= items[i];
		const std::string	 &itemValue = item.getValue();
		if (conceptPaths.contains(itemValue))
			continue;
		if (!item.isTerminalChar())
			getPathsTerminalChars(completePath, itemValue, mapPath, conceptPaths);
		else
		{
			mapPath[rule].push_once(ARulePathInfo(completePath));
		}
	}

	const Set<const ARule *>		  &terminalChars = m_semantic.getConcept(conceptName)->getTerminalChars();
	Set<const ARule *>::const_iterator ite			 = terminalChars.end();
	for (Set<const ARule *>::const_iterator it = terminalChars.begin(); it != ite; ++it)
	{
		if (rule == *it)
		{
			mapPath[rule].push_once(ARulePathInfo(completePath));
		}
	}
	completePath.pop_back();
}

void Parser::addSequencePaths(const ARulePath &path, ARuleMap &mapPathNext)
{
	for (int i = path.size() - 1; i >= 0; --i)
	{
		const ARule *const rule = path[i];
		if (rule->isSequence())
		{
			const Vec<TerminalSymbol>		   &items = rule->getItems();
			Vec<TerminalSymbol>::const_iterator ite	  = items.end();
			for (Vec<TerminalSymbol>::const_iterator terminalSymbol = items.begin(); terminalSymbol != ite; ++terminalSymbol)
			{
				if (terminalSymbol->isTerminalChar())
				{
					mapPathNext[rule].push_once(ARulePathInfo(path.slice(i + 1)));
				}
				else
				{
					Vec<std::string> emptyConceptPaths;
					emptyConceptPaths.reserve(8);
					ARulePath subPath = path.slice(i + 1);

					getPathsTerminalChars(subPath, terminalSymbol->getValue(), mapPathNext, emptyConceptPaths);
				}
			}
		}

		const ARule *adjacentRule = rule->tryGetAdjacentRules();
		while (adjacentRule && isFullyOptional(*adjacentRule))
			adjacentRule = adjacentRule->tryGetAdjacentRules();
		if (adjacentRule)
			return;
	}
}

void Parser::getPathsTerminalChars(ARulePath &completePath, const std::string &conceptName, ARuleMap &mapPath, Vec<std::string> &conceptPaths)
{
	conceptPaths.push_back(conceptName);
	const Vec<ConceptDef *> &defs			 = m_semantic.getConceptDefs(conceptName);
	size_t					 conceptDefCount = defs.size();
	for (size_t i = 0; i < conceptDefCount; ++i)
	{
		const ConceptDef *def  = defs[i];
		size_t			  size = def->size();
		size_t			  k	   = 0;
		while (k < size)
		{
			const ARule *subRule = def->getARule(k);
			addPathPossibility(subRule, conceptPaths, mapPath, conceptName, completePath);
			if (!isFullyOptional(*subRule))
				break;
			++k;
		}
	}
	conceptPaths.pop_back();
}

void Parser::addNextPaths(const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext, const ARulePath &path, const ARule *const rule)
{
	addSequencePaths(path, mapPathNext);
	ARulePath nextRule = rule->getNextRules(path);
	if (nextRule.empty())
		return;
	const ARule *const lastRule = nextRule.back();
	if (isFullyOptional(*lastRule))
		addNextPaths(mapPathCurrent, mapPathNext, nextRule, lastRule);

	const Vec<TerminalSymbol> &items	  = lastRule->getItems();
	size_t					   itemsCount = items.size();
	for (size_t i = 0; i < itemsCount; ++i)
	{
		const TerminalSymbol &terminalSymbol = items[i];
		if (terminalSymbol.isTerminalChar())
			mapPathNext[lastRule].push_once(ARulePathInfo(nextRule));
		else
		{
			Vec<std::string> emptyConceptPaths;
			emptyConceptPaths.reserve(8);
			getPathsTerminalChars(nextRule, terminalSymbol.getValue(), mapPathNext, emptyConceptPaths);
		}
	}
}

bool Parser::word(
	TerminalCharInfo  &data,
	const char		   c,
	const ARule *const rule,
	const ARulePath	  &path,
	const ARuleMap	  &mapPathCurrent,
	ARuleMap		  &mapPathNext)
{
	if (data.position >= data.length)
		return false;

	if (data.terminal->getValue()[data.position] != c)
		return false;

	if (data.position == data.length - 1)
	{
		addNextPaths(mapPathCurrent, mapPathNext, path, rule);
		return true;
	}

	++data.position;

	ARulePaths &paths	  = mapPathNext[rule];
	size_t		pathCount = paths.size();

	for (size_t i = 0; i < pathCount; ++i)
	{
		ARulePathInfo &it = paths[i];
		if (it.path == path)
		{
			it.terminalCharsInfo.push_back(data);
			return true;
		}
	}

	ARulePathInfo pathInfoObj(path);
	pathInfoObj.terminalCharsInfo.reserve(1);
	pathInfoObj.terminalCharsInfo.push_back(data);
	paths.push_back(pathInfoObj);

	return true;
}


bool Parser::parseCurrentChar(
	const char		c,
	const ARuleMap &mapPathCurrent,
	ARuleMap	   &mapPathNext)
{
	bool isValidWithCurrentPath = false;

	for (ARuleMap::const_iterator it = mapPathCurrent.begin();
		 it != mapPathCurrent.end(); ++it)
	{
		const ARule				  *rule	 = it->first;
		const ARulePaths		  &paths = it->second;
		const Vec<TerminalSymbol> &items = rule->getItems();

		size_t pathsCount = paths.size();
		size_t itemsCount = items.size();

		for (size_t pathIndex = 0; pathIndex < pathsCount; ++pathIndex)
		{
			size_t indexElementTerminalChar = 0;

			for (size_t itemIndex = 0; itemIndex < itemsCount; ++itemIndex)
			{
				const TerminalSymbol &item = items[itemIndex];

				if (!item.isTerminalChar())
					continue;

				TerminalCharInfo data;
				if (paths[pathIndex].terminalCharsInfo.has(indexElementTerminalChar))
				{
					data = paths[pathIndex].terminalCharsInfo[indexElementTerminalChar];
				}
				else
				{
					data = TerminalCharInfo(&item, 0);
				}

				if (word(data, c, rule, paths[pathIndex].path, mapPathCurrent, mapPathNext))
					isValidWithCurrentPath = true;
				++indexElementTerminalChar;
			}
		}
	}
	return isValidWithCurrentPath;
}

bool Parser::isFullyOptional(const ARule &rule) const
{
	std::map<const ARule *, bool>::iterator cacheIt = m_optionalCache.find(&rule);
	if (cacheIt != m_optionalCache.end())
		return cacheIt->second;

	if (m_optionalComputing.find(&rule) != m_optionalComputing.end())
		return false;

	m_optionalComputing.insert(&rule);

	bool result = false;

	if (rule.isOption())
	{
		result = true;
	}
	else
	{
		const Vec<TerminalSymbol> &items = rule.getItems();

		for (Vec<TerminalSymbol>::const_iterator item = items.begin();
			 item != items.end();
			 ++item)
		{
			if (item->isTerminalChar())
				continue;

			Concept					*cpt  = m_semantic.getConcepts().get(item->getValue());
			const Vec<ConceptDef *> &defs = cpt->getDefinitions();

			for (Vec<ConceptDef *>::const_iterator cdef = defs.begin();
				 cdef != defs.end();
				 ++cdef)
			{
				const ARulePath &rules = (*cdef)->getRules();

				bool allOptional = true;

				for (size_t k = 0; k < rules.size(); ++k)
				{
					if (!isFullyOptional(*(rules[k])))
					{
						allOptional = false;
						break;
					}
				}

				if (allOptional)
				{
					result = true;
					break;
				}
			}

			if (result)
				break;
		}
	}

	m_optionalCache[&rule] = result;
	m_optionalComputing.erase(&rule);

	return result;
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
		const ARulePaths &val = it->second;
		for (ARulePaths::const_iterator path = val.begin(); path != val.end(); ++path)
		{
			const ARulePath			  &rules = path->path;
			const Vec<TerminalSymbol> &items = rules.back()->getItems();
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


void Parser::precomputeOptional(const ARule *rule)
{
	if (m_optionalCache.find(rule) != m_optionalCache.end())
		return;

	if (m_optionalComputing.find(rule) != m_optionalComputing.end())
	{
		m_optionalCache[rule] = false;
		return;
	}
	m_optionalComputing.insert(rule);

	bool result = false;

	if (rule->isOption())
	{
		result = true;
	}
	else
	{
		const Vec<TerminalSymbol> &items = rule->getItems();
		result							 = true;
		for (size_t i = 0; i < items.size(); ++i)
		{
			const TerminalSymbol &item = items[i];

			if (item.isTerminalChar())
			{
				result = false;
				break;
			}

			Concept					*cpt  = m_semantic.getConcepts().get(item.getValue());
			const Vec<ConceptDef *> &defs = cpt->getDefinitions();

			bool anyDefOptional = false;
			for (size_t j = 0; j < defs.size(); ++j)
			{
				const ARulePath &rules			  = defs[j]->getRules();
				bool			 allRulesOptional = true;
				for (size_t k = 0; k < rules.size(); ++k)
				{
					precomputeOptional(rules[k]);
					if (!m_optionalCache[rules[k]])
					{
						allRulesOptional = false;
						break;
					}
				}
				if (allRulesOptional)
				{
					anyDefOptional = true;
					break;
				}
			}

			if (!anyDefOptional)
			{
				result = false;
				break;
			}
		}
	}

	m_optionalCache[rule] = result;
	m_optionalComputing.erase(rule);
}

void Parser::parse(const std::string &stringToParsed)
{
	if (stringToParsed.empty())
	{
		if (!isValidEmptyFile())
			throw FileNotFinishedException("The file must not be empty.");
		return;
	}

	const Vec<ConceptDef *> &defs = m_semantic.getConceptDefs(CURSOR);
	for (size_t i = 0; i < defs.size(); ++i)
	{
		const ARule *startRule = defs[i]->getARule(0);
		precomputeOptional(startRule);
	}

	ARuleMap mapPathCurrent;
	ARuleMap mapPathNext;
	char	 c;

	Vec<std::string> emptyConceptPaths;
	emptyConceptPaths.reserve(8);

	ARulePath completePath;
	completePath.reserve(8);

	getPathsTerminalChars(completePath, CURSOR, mapPathCurrent, emptyConceptPaths);

	for (size_t i = 0; i < stringToParsed.size(); ++i)
	{
		c = stringToParsed[i];
		if (!parseCurrentChar(c, mapPathCurrent, mapPathNext))
			throw InvalidEntryException(std::string("Invalid character '") + c + "' at position " + Logger::get(i) + ".");
		if (i == stringToParsed.size() - 1)
			break;

		std::swap(mapPathCurrent, mapPathNext);
		mapPathNext.clear();
	}

	if (mapPathNext.empty() || verifyOptionalEnd(mapPathCurrent, c))
		Logger::log("Parsing complete ✅");
	else
		throw FileNotFinishedException("END: The file ended before reaching a valid end state.");
}
}