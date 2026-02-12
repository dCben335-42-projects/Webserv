#include <sstream>
#include <iostream>
#include "./Parser.hpp"
#include "../ARulePathInfo/ARulePathInfo.hpp"
#include "Json.hpp"
#include "../BNFPath/BNFPath.hpp"
#include "../DataCollector/DataCollector.hpp"

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
	ARulePath		   prefix)
{
	Vec<TerminalSymbol> items = rule->getItems();
	path.push_back(rule);
	for (Vec<TerminalSymbol>::iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
	{
		std::string item = itemIt->getValue();
		if (conceptPaths.contains(item))
			continue;
		if (!itemIt->isTerminalChar())
		{
			getPathsTerminalChars(prefix, item, mapPath, conceptPaths, path);
		}
		else
		{
			if (!mapPath.has(rule))
				mapPath.set(rule, ARulePaths());
			ARulePath newPath = ARulePath::concat(prefix, path);
			mapPath.get(rule).push_once(ARulePathInfo(newPath));
			if (m_originelRules.has(*m_currentRule))
				m_originelRules.get(*m_currentRule).push_once(newPath);
		}
	}

	Set<const ARule *> terminalChars = m_semantic.getConcept(conceptName)->getTerminalChars();
	for (Set<const ARule *>::iterator it = terminalChars.begin(); it != terminalChars.end(); ++it)
	{
		if (rule == *it)
		{
			if (!mapPath.has(rule))
				mapPath.set(rule, ARulePaths());
			ARulePath newPath = ARulePath::concat(prefix, path);
			mapPath.get(rule).push_once(ARulePathInfo(newPath));
			if (m_originelRules.has(*m_currentRule))
				m_originelRules.get(*m_currentRule).push_once(newPath);
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
					mapPathNext.get(path[i]).push_once(ARulePathInfo(path.slice(i + 1)));
					if (m_originelRules.has(*m_currentRule))
						m_originelRules.get(*m_currentRule).push_once(path.slice(i + 1));
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

void Parser::getPathsTerminalChars(ARulePath prefix, std::string conceptName, ARuleMap &mapPath, Vec<std::string> conceptPaths, ARulePath path)
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
	if (isFullyOptional(*lastRule))
		addNextPaths(mapPathCurrent, mapPathNext, nextRule, lastRule);

	V_FOREACH(TerminalSymbol, terminalSymbol, lastRule->getItems())
	{
		if (terminalSymbol->isTerminalChar())
		{
			if (!mapPathNext.has(lastRule))
				mapPathNext.set(lastRule, ARulePaths());
			mapPathNext.get(lastRule).push_once(ARulePathInfo(nextRule));
			if (m_originelRules.has(*m_currentRule))
				m_originelRules.get(*m_currentRule).push_once(nextRule);
		}
		else
		{
			getPathsTerminalChars(nextRule, terminalSymbol->getValue(), mapPathNext, Vec<std::string>(), ARulePath());
		}
	}
}

bool Parser::word(
	TerminalCharInfo  *data,
	const char		   c,
	size_t			   idx,
	const ARule *const rule,
	ARulePathInfo	  *pathInfo,
	const ARuleMap	  &mapPathCurrent,
	ARuleMap		  &mapPathNext)
{

	if (data->position >= data->length)
		return false;

	if (data->terminal->getValue()[data->position] == c)
	{
		if (data->position == data->length - 1)
		{
			if (!m_originelRules.has(pathInfo->path))
				m_originelRules.set(pathInfo->path, Vec<ARulePath>());
			m_currentRule = &pathInfo->path;
			// Logger::log("Matched path: " + LoggerJson::get(data->terminal->getValue()));
			// m_lines.push_back(data->terminal->getValue());

			if ((int) idx - (int) data->position - 1 >= 0)
			{
				// Logger::log(idx - data->position - 1);
				m_rules[idx - data->position - 1].push(pathInfo->path);
			}

			addNextPaths(mapPathCurrent, mapPathNext, pathInfo->path, rule);
		}
		else
		{
			if (!m_originelRules.has(pathInfo->path))
				m_originelRules.set(pathInfo->path, Vec<ARulePath>());
			m_originelRules.get(pathInfo->path).push_once(pathInfo->path);

			data->position += 1;
			if (!mapPathNext.has(rule))
				mapPathNext.set(rule, ARulePaths());

			ARulePaths &mapPath = mapPathNext.get(rule);

			for (size_t i = 0; i < mapPath.size(); ++i)
			{
				ARulePathInfo &it = mapPath[i];
				if (it.path == pathInfo->path)
				{
					it.terminalCharsInfo.push(*data);
					return true;
				}
			}
			ARulePathInfo pathInfoObj(pathInfo->path);
			pathInfoObj.terminalCharsInfo.push(*data);
			mapPathNext.get(rule).push_once(pathInfoObj);
		}
		return true;
	}
	return false;
}

bool Parser::parseCurrentChar(const char c, const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext, size_t idx)
{
	bool isValidWithCurrentPath = false;
	for (ARuleMap::const_iterator it = mapPathCurrent.begin(); it != mapPathCurrent.end(); ++it)
	{
		const ARule				  *rule	 = it->first;
		ARulePaths				   paths = it->second;
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

				if (word(&data, c, idx, rule, &paths[pathIndex], mapPathCurrent, mapPathNext))
					isValidWithCurrentPath = true;
				indexElementTerminalChar++;
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
		Concept			 *cpt = m_semantic.getConcepts().get(item->getValue());
		Vec<ConceptDef *> def = cpt->getDefinitions();

		for (Vec<ConceptDef *>::iterator cdef = def.begin(); cdef != def.end(); ++cdef)
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

void Parser::completePath()
{
	m_indexedPaths.push(m_originelRules);
	m_originelRules.clear();
}

std::ostream &operator<<(std::ostream &os, const BNFPath &data)
{
	os << data.next.size();
	return os;
}

const BNFPath *Parser::resolvePath()
{
	// clang-format off
	Vec<BNFPath *> basePaths;
	Vec<BNFPath *> pathsTop;
	Map<ARulePath, Vec<ARulePath> > *mapPrev = &m_indexedPaths[0];

	Logger::log("size" + Logger::get(m_indexedPaths.size()));
	typedef Map<ARulePath, Vec<ARulePath> >::const_iterator MapIt;
	for (MapIt it = mapPrev->begin(); it != mapPrev->end(); ++it)
	{
		basePaths.push_once(new BNFPath(it->first, 0, m_rules[0]));
	}
	pathsTop	 = basePaths;
	for (size_t k = 1; k < m_indexedPaths.size(); ++k)
	{
		Map<ARulePath, Vec<ARulePath> > *mapCurrent = &m_indexedPaths[k];

		Vec<const ARulePath *>						   vecKey;
		Map<const ARulePath *, Vec<const ARulePath *> > mapValueToKey;
		for (MapIt it = mapCurrent->begin(); it != mapCurrent->end(); ++it)
		{
			vecKey.push_once(&it->first);
		}

		for (MapIt it = mapPrev->begin(); it != mapPrev->end(); ++it)
		{
			const ARulePath &ident = it->first;
			for (size_t i = 0; i < it->second.size(); ++i)
			{
				const ARulePath &vecValue = it->second[i];
				for (size_t j = 0; j < vecKey.size(); ++j)
				{
					bool found		 = false;
					const ARulePath &vecKeyValue = *vecKey[j];

					if (!BNFPath::compare(vecValue, vecKeyValue))
						continue;
					
					for (size_t l = 0; l < pathsTop.size(); ++l)
					{
						if (pathsTop[l]->rulePath == ident)
						{
							if (found)
								throw std::runtime_error("Ambiguous path resolution.");
							pathsTop[l]->setNext(vecKeyValue, k, m_rules[k]);
							found = true;
						}
					}
				}
			}
		}

		Vec<BNFPath *> bufferTemp;
		for (int i = 0; i < static_cast<int>(pathsTop.size()); ++i)
		{
			BNFPath &current = *pathsTop[i];

			if (current.next.empty())
			{
				pathsTop.remove(i);
				current.deleteBranch();
				i--;
				continue;
			}

			const size_t size = current.next.size();
			pathsTop[i]		  = &(current.next[0]);
			for (size_t j = 1; j < size; ++j)
				bufferTemp.push_once(&(current.next[j]));
		}

		if (pathsTop.size() < 1)
			throw std::runtime_error("All paths have been invalidated. The input string does not conform to the grammar.");

		pathsTop.concat(bufferTemp);
		mapPrev = mapCurrent;
	}

	

	return pathsTop[0]->getHead();
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

	m_rules.resize(stringToParsed.size());

	m_currentRule = NULL;
	getPathsTerminalChars(ARulePath(), CURSOR, mapPathCurrent, Vec<std::string>(), ARulePath());

	for (size_t i = 0; i < stringToParsed.size(); ++i)
	{
		c = stringToParsed[i];
		if (!parseCurrentChar(c, mapPathCurrent, mapPathNext, i))
			throw InvalidEntryException(std::string("Invalid character '") + c);
		completePath();
		if (i == stringToParsed.size() - 1)
			break;

		mapPathCurrent = mapPathNext;
		mapPathNext.clear();
	}
	
	if (mapPathNext.empty() || verifyOptionalEnd(mapPathCurrent, c)) // Pas tout a fait correct si il existe une suite pleinement optionnele cest aussi valide
		Logger::log("Parsing complete ✅");
	else
		throw FileNotFinishedException("END: The file ended before reaching a valid end state.");
	DataCollector dataCollector;
	dataCollector.generateJson(resolvePath(), stringToParsed, m_lines);
}