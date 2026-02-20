#include <sstream>
#include <iostream>
#include "./Parser.hpp"
#include "../ARulePathInfo/ARulePathInfo.hpp"
#include "Json.hpp"
#include "../BNFPath/BNFPath.hpp"

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
	const std::string &conceptName,
	const ARulePath	  &prefix)
{
	const Vec<TerminalSymbol> &items = rule->getItems();
	path.push_back(rule);
	for (Vec<TerminalSymbol>::const_iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
	{
		const std::string &item = itemIt->getValue();
		if (conceptPaths.contains(item))
			continue;
		if (!itemIt->isTerminalChar())
			getPathsTerminalChars(prefix, item, mapPath, conceptPaths, path);
		else
			mapPath[rule].push_once(ARulePathInfo(ARulePath::concat(prefix, path)));
	}

	const Set<const ARule *> &terminalChars = m_semantic.getConcept(conceptName)->getTerminalChars();
	for (Set<const ARule *>::const_iterator it = terminalChars.begin(); it != terminalChars.end(); ++it)
	{
		if (rule == *it)
			mapPath[rule].push_once(ARulePathInfo(ARulePath::concat(prefix, path)));
	}
}

void Parser::addSequencePaths(const ARulePath &path, ARuleMap &mapPathNext)
{
	for (int i = path.size() - 1; i >= 0; --i)
	{
		const ARule *rule = path[i];
		if (rule->isSequence())
		{
			const ARulePath			  &slicePath = path.slice(i + 1);
			const Vec<TerminalSymbol> &items	 = rule->getItems();
			for (Vec<TerminalSymbol>::const_iterator terminalSymbol = items.begin(); terminalSymbol != items.end(); ++terminalSymbol)
			{
				if (terminalSymbol->isTerminalChar())
				{
					mapPathNext[rule].push_once(ARulePathInfo(slicePath));
				}
				else
					getPathsTerminalChars(slicePath, terminalSymbol->getValue(), mapPathNext, Vec<std::string>(), ARulePath());
			}
		}

		const ARule *adjacentRule = rule->tryGetAdjacentRules();
		while (adjacentRule)
		{
			if (isFullyOptional(*adjacentRule))
				adjacentRule = adjacentRule->tryGetAdjacentRules();
			else
				return;
		}
	}
}


// void Parser::addSequencePaths(const ARulePath &path, ARuleMap &mapPathNext)
// {
// 	// Parcours inverse
// 	for (int i = path.size() - 1; i >= 0; --i)
// 	{
// 		const ARule *currentRule = path[i];

// 		if (currentRule->isSequence())
// 		{
// 			const Vec<TerminalSymbol> &items = currentRule->getItems();
// 			ARulePath				   subPath;
// 			for (size_t j = i + 1; j < path.size(); ++j)
// 				subPath.push_back(path[j]);

// 			for (Vec<TerminalSymbol>::const_iterator it = items.begin(); it != items.end(); ++it)
// 			{
// 				const TerminalSymbol &terminalSymbol = *it;

// 				if (terminalSymbol.isTerminalChar())
// 				{
// 					if (!mapPathNext.has(currentRule))
// 						mapPathNext.set(currentRule, ARulePaths());

// 					ARulePaths &paths = mapPathNext.get(currentRule);
// 					paths.push_once(ARulePathInfo(subPath));
// 				}
// 				else
// 				{
// 					// Réutiliser subPath au lieu de reconstruire à chaque appel
// 					getPathsTerminalChars(subPath, terminalSymbol.getValue(), mapPathNext, Vec<std::string>(), ARulePath());
// 				}
// 			}
// 		}

// 		// Gestion des règles adjacentes optionnelles
// 		const ARule *adjacentRule = currentRule->tryGetAdjacentRules();
// 		while (adjacentRule)
// 		{
// 			if (isFullyOptional(*adjacentRule))
// 				adjacentRule = adjacentRule->tryGetAdjacentRules();
// 			else
// 				break; // on arrête la propagation
// 		}
// 	}
// }

void Parser::getPathsTerminalChars(const ARulePath &prefix, const std::string &conceptName, ARuleMap &mapPath, Vec<std::string> conceptPaths, ARulePath path)
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

void Parser::addNextPaths(const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext, const ARulePath &path, const ARule *const rule)
{
	addSequencePaths(path, mapPathNext);
	const ARulePath &nextRule = rule->getNextRules(path);
	if (nextRule.empty())
		return;

	const ARule *const lastRule = nextRule.back();
	if (isFullyOptional(*lastRule))
		addNextPaths(mapPathCurrent, mapPathNext, nextRule, lastRule);

	V_FOREACH(TerminalSymbol, terminalSymbol, lastRule->getItems())
	{
		if (terminalSymbol->isTerminalChar())
		{
			ARulePaths &paths = mapPathNext[lastRule];
			paths.push_once(ARulePathInfo(nextRule));
		}
		else
			getPathsTerminalChars(nextRule, terminalSymbol->getValue(), mapPathNext, Vec<std::string>(), ARulePath());
	}
}

bool Parser::word(
	TerminalCharInfo  *data,
	const char		   c,
	const ARule *const rule,
	const ARulePath	  &path,
	const ARuleMap	  &mapPathCurrent,
	ARuleMap		  &mapPathNext)
{

	if (data->position >= data->length)
		return false;

	if (data->terminal->getValue()[data->position] == c)
	{
		if (data->position == data->length - 1)
		{
			addNextPaths(mapPathCurrent, mapPathNext, path, rule);
		}
		else
		{
			data->position += 1;

			ARulePaths &paths = mapPathNext[rule];

			for (size_t i = 0; i < paths.size(); ++i)
			{
				ARulePathInfo &it = paths[i];
				if (it.path == path)
				{
					it.terminalCharsInfo.push(*data);
					return true;
				}
			}
			ARulePathInfo pathInfoObj(path);
			pathInfoObj.terminalCharsInfo.push(*data);
			paths.push_once(pathInfoObj);
		}
		return true;
	}
	return false;
}

bool Parser::parseCurrentChar(const char c, const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext)
{
	bool isValidWithCurrentPath = false;
	for (ARuleMap::const_iterator it = mapPathCurrent.begin(); it != mapPathCurrent.end(); ++it)
	{
		const ARule				  *rule	 = it->first;
		const ARulePaths		  &paths = it->second;
		const Vec<TerminalSymbol> &items = rule->getItems();
		for (size_t pathIndex = 0; pathIndex < paths.size(); ++pathIndex)
		{
			size_t indexElementTerminalChar = 0;
			for (size_t itemIndex = 0; itemIndex < items.size(); ++itemIndex)
			{
				if (!items[itemIndex].isTerminalChar())
					continue;

				TerminalCharInfo data;
				if (!(paths[pathIndex].terminalCharsInfo.has(indexElementTerminalChar)))
				{
					data = TerminalCharInfo(&items[itemIndex], 0);
				}
				else
				{
					data = paths[pathIndex].terminalCharsInfo.at(indexElementTerminalChar);
				}

				if (word(&data, c, rule, paths[pathIndex].path, mapPathCurrent, mapPathNext))
					isValidWithCurrentPath = true;
				indexElementTerminalChar++;
			}
		}
	}
	return isValidWithCurrentPath;
}


// bool Parser::isFullyOptional(const ARule &rule) const
// {
// 	if (rule.isOption())
// 		return true;
// 	Vec<TerminalSymbol> items = rule.getItems();
// 	for (Vec<TerminalSymbol>::const_iterator item = items.begin(); item != items.end(); ++item)
// 	{
// 		if (item->isTerminalChar())
// 			continue;
// 		Concept			 *cpt = m_semantic.getConcepts().get(item->getValue());
// 		Vec<ConceptDef *> def = cpt->getDefinitions();

// 		for (Vec<ConceptDef *>::iterator cdef = def.begin(); cdef != def.end(); ++cdef)
// 		{
// 			ARulePath rules				= (*cdef)->getRules();
// 			size_t	  isOptionalCounter = 0;

// 			for (size_t k = 0; k < rules.size(); ++k)
// 				isOptionalCounter += isFullyOptional(*(rules[k]));
// 			if (isOptionalCounter == rules.size())
// 				return true;
// 		}
// 	}
// 	return false;
// }

bool Parser::isFullyOptional(const ARule &rule) const
{
	// 🔹 Cache lookup
	std::map<const ARule *, bool>::iterator cacheIt = m_optionalCache.find(&rule);
	if (cacheIt != m_optionalCache.end())
		return cacheIt->second;

	// 🔹 Cycle detection
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

	// 🔹 Store in cache
	m_optionalCache[&rule] = result;

	// 🔹 Remove from computing set
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
		const ARulePaths val = it->second;
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

std::ostream &operator<<(std::ostream &os, const BNFPath &data)
{
	os << data.next.size();
	return os;
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

	for (size_t i = 0; i < stringToParsed.size(); ++i)
	{
		c = stringToParsed[i];
		if (!parseCurrentChar(c, mapPathCurrent, mapPathNext))
			throw InvalidEntryException(std::string("Invalid character '") + c + "' at position " + Logger::get(i) + ".");
		if (i == stringToParsed.size() - 1)
			break;


		std::swap(mapPathCurrent, mapPathNext);
		// mapPathCurrent = mapPathNext;
		mapPathNext.clear();
	}

	if (mapPathNext.empty() || verifyOptionalEnd(mapPathCurrent, c))
		Logger::log("Parsing complete ✅");
	else
		throw FileNotFinishedException("END: The file ended before reaching a valid end state.");
}