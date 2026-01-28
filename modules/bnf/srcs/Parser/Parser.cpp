#include <sstream>
#include <iostream>
#include "./Parser.hpp"
#include "../ARulePathInfo/ARulePathInfo.hpp"
#include "Json.hpp"

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
			if (m_originelRules.has(*currentRule))
				m_originelRules.get(*currentRule).push_once(newPath);
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
			if (m_originelRules.has(*currentRule))
				m_originelRules.get(*currentRule).push_once(newPath);
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
					if (m_originelRules.has(*currentRule))
						m_originelRules.get(*currentRule).push_once(path.slice(i + 1));
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
			if (m_originelRules.has(*currentRule))
				m_originelRules.get(*currentRule).push_once(nextRule);
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
			currentRule = &pathInfo->path;
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

bool Parser::parseCurrentChar(const char c, const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext)
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

				if (word(&data, c, rule, &paths[pathIndex], mapPathCurrent, mapPathNext))
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

void Parser::completePath()
{
	m_indexedPaths.push(m_originelRules);
	m_originelRules.clear();
}





class BNFPath
{
	public:
		BNFPath(const ARulePath &p) : rulePath(p), prev(NULL)
		{
		}
		BNFPath(const ARulePath &p, BNFPath *pr) : rulePath(p), prev(pr)
		{
		}
		~BNFPath()
		{
		}
		const ARulePath &rulePath;
		BNFPath			*prev;
		Vec<BNFPath>	 next;

		void setNext(const ARulePath &p)
		{
			next.push(BNFPath(p, this));
		}
};

void generateJson()
{
}


void Parser::resolvePath()
{
	Vec<BNFPath *> basePaths;
	Vec<BNFPath *> pathsTop;
	// clang-format off
	Map<ARulePath, Vec<ARulePath> > &mapPrev = m_indexedPaths[0];
	for (Map<ARulePath, Vec<ARulePath> >::const_iterator it = mapPrev.begin(); it != mapPrev.end(); ++it)
	{
		basePaths.push_once(new BNFPath(it->first));
	}
	pathsTop	 = basePaths;
	for (size_t k = 1; k < m_indexedPaths.size(); ++k)
	{
		Logger::log("======== Resolving index: " + LoggerJson::get(k) + " ========");
		Map<ARulePath, Vec<ARulePath> > &mapCurrent = m_indexedPaths[k];

		Vec<const ARulePath *>						   vecKey;
		Map<const ARulePath *, Vec<const ARulePath *> > mapValueToKey;
		for (Map<ARulePath, Vec<ARulePath> >::iterator it = mapCurrent.begin(); it != mapCurrent.end(); ++it)
		{
			vecKey.push_once(&it->first);
		}

		for (Map<ARulePath, Vec<ARulePath> >::iterator it = mapPrev.begin(); it != mapPrev.end(); ++it)
		{
			const ARulePath &ident = it->first;
			for (size_t i = 0; i < it->second.size(); ++i)
			{
				const ARulePath &vecValue = it->second[i];
				for (size_t j = 0; j < vecKey.size(); ++j)
				{
					bool found		 = false;
					if (vecValue.back() != vecKey[j]->back())
						continue;
					for (size_t l = 0; l < pathsTop.size(); ++l)
					{
						if (pathsTop[l]->rulePath == ident)
						{
							if (found)
								throw std::runtime_error("Ambiguous path resolution.");
							pathsTop[l]->setNext(*vecKey[j]);
							found = true;
						}
					}
				}
			}
		}

		Vec<BNFPath *> bufferTemp;
		Logger::log("original pathsTop size: " + LoggerJson::get(pathsTop.size()));
		for (int i = 0; i < static_cast<int>(pathsTop.size()); ++i)
		{
			BNFPath &current = *pathsTop[i];
			if (current.next.empty())
			{
				pathsTop.remove(i);
				i--;
				continue;
			}

			const size_t size = current.next.size();
			pathsTop[i]		  = &(current.next[0]);
			for (size_t j = 1; j < size; ++j)
				bufferTemp.push_once(&(current.next[j]));
		}
		pathsTop.concat(bufferTemp);
		mapPrev = mapCurrent;
	}
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

	currentRule = NULL;

	Logger::log(m_semantic);
	getPathsTerminalChars(ARulePath(), CURSOR, mapPathCurrent, Vec<std::string>(), ARulePath());
	// for (ARuleMap::iterator it = mapPathCurrent.begin(); it != mapPathCurrent.end(); ++it)
	// {
	// 	for (size_t i = 0; i < it->second.size(); ++i)
	// 	{
	// 		m_startingPaths.push_once(it->second[i].path);
	// 	}
	// }



	for (size_t i = 0; i < stringToParsed.size(); ++i)
	{
		c = stringToParsed[i];
		// Logger::log("index: " + LoggerJson::get(i) + ", char: '" + c + "'");
		if (!parseCurrentChar(c, mapPathCurrent, mapPathNext))
			throw InvalidEntryException(std::string("Invalid character '") + c);
		// else
		// Logger::log("✅");
		if (i == stringToParsed.size() - 1)
			break;
		completePath();

		mapPathCurrent = mapPathNext;
		mapPathNext.clear();
	}
	// clang-format off
	for (size_t k = 0; k < m_indexedPaths.size(); ++k)
	{
		Logger::log("Index: " + LoggerJson::get(k));
		for (Map<ARulePath, Vec<ARulePath> >::iterator mapIt = m_indexedPaths[k].begin(); mapIt != m_indexedPaths[k].end(); ++mapIt)
		{
			Logger::log("----");
			Logger::log("\tKey: " + LoggerJson::get(mapIt->first, true));
			for (Vec<ARulePath>::iterator pathIt = mapIt->second.begin(); pathIt != mapIt->second.end(); ++pathIt)
			{
				Logger::log("\t\tValue: " + LoggerJson::get(pathIt->back(), true));
			}
		}
	}
	resolvePath();


	if (mapPathNext.empty() || verifyOptionalEnd(mapPathCurrent, c)) // Pas tout a fait correct si il existe une suite pleinement optionnele cest aussi valide
		Logger::log("Parsing complete ✅");
	else
		throw FileNotFinishedException("END: The file ended before reaching a valid end state.");
}

std::ostream &operator<<(std::ostream &os, const BNFPath &data)
{
	// Logger::log(LoggerJson::get(*(data.rulePath)));
	os << LoggerJson::get((data.rulePath));
	return (os);
}