#include <sstream>
#include <iostream>
#include <algorithm>
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
			PathCacheKey key(conceptName, completePath);
			mapPath[rule].push_once(ARulePathInfo(completePath));
			// m_pathsCache[key][rule].push_once(ARulePathInfo(completePath));

			if (!m_pathsCache.has(key))
				m_pathsCache.set(key, ARuleMap());
			ARuleMap &cachedMap = m_pathsCache.get(key);
			if (!cachedMap.has(rule))
				cachedMap.set(rule, ARulePaths());
			cachedMap.get(rule).push_once(ARulePathInfo(completePath));
		}
	}

	const Set<const ARule *>		  &terminalChars = m_semantic.getConcept(conceptName)->getTerminalChars();
	Set<const ARule *>::const_iterator ite			 = terminalChars.end();
	for (Set<const ARule *>::const_iterator it = terminalChars.begin(); it != ite; ++it)
	{
		if (rule == *it)
		{
			PathCacheKey key(conceptName, completePath);
			mapPath[rule].push_once(ARulePathInfo(completePath));

			if (!m_pathsCache.has(key))
				m_pathsCache.set(key, ARuleMap());
			ARuleMap &cachedMap = m_pathsCache.get(key);
			if (!cachedMap.has(rule))
				cachedMap.set(rule, ARulePaths());
			cachedMap.get(rule).push_once(ARulePathInfo(completePath));
			// Logger::log(m_pathsCache[key][rule]);
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
					// m_pathsCache[PathCacheKey{terminalSymbol->getValue(), path.slice(i + 1)}][rule].push_once(ARulePathInfo(path.slice(i + 1)));
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

PathCacheKey::PathCacheKey()
{
}

PathCacheKey::PathCacheKey(const std::string &name, const ARulePath &path) : conceptName(name), pathContext(path)
{
}

bool PathCacheKey::operator==(const PathCacheKey &other) const
{
	Logger::debug("Comparing PathCacheKey: " + Logger::get(*this) + " with " + Logger::get(other));
	return conceptName == other.conceptName && pathContext == other.pathContext;
}

bool PathCacheKey::operator<(const PathCacheKey &other) const
{
	return (pathContext < other.pathContext);
}

void Parser::getPathsTerminalChars(ARulePath &completePath, const std::string &conceptName, ARuleMap &mapPath, Vec<std::string> &conceptPaths)
{

	// if (m_pathsCache.has(PathCacheKey{conceptName, completePath}))
	// {
	// 	mapPath = m_pathsCache[PathCacheKey{conceptName, completePath}];
	// }

	PathCacheKey key(conceptName, completePath);

	// Logger::debug("Trying cache for concept: " + conceptName + " path size: " + Logger::get(completePath.size()));
	// for (size_t i = 0; i < completePath.size(); ++i)
	// 	Logger::debug("Rule pointer: " + Logger::get((size_t) completePath[i]));
	// Logger::error("--------------------------------------------------------------------------------");
	// Logger::error("--------------------------------------------------------------------------------");
	// Logger::error("--------------------------------------------------------------------------------");
	// Logger::error("--------------------------------------------------------------------------------");
	// Logger::debug("Cache key: " + Logger::get(key));
	// static bool hasLoggedCacheContent = false;
	// if (m_pathsCache.size() == 305 && !hasLoggedCacheContent)
	// {
	// 	hasLoggedCacheContent = true;
	// 	Logger::log("Cache content: " + LoggerJson::get(m_pathsCache, true)).log("Key: " + Logger::get(key));
	// 	Logger::debug(m_pathsCache);
	// }

	// if (m_pathsCache.has(key))
	// {
	// 	Logger::debug("Cache hit for concept: " + conceptName);
	// 	const ARuleMap &cachedMap = m_pathsCache[key];

	// 	for (std::map<const ARule *, ARulePaths>::const_iterator it = cachedMap.begin();
	// 		 it != cachedMap.end(); ++it)
	// 	{
	// 		const ARule		 *rule		  = it->first;
	// 		const ARulePaths &cachedPaths = it->second;

	// 		// On ajoute les chemins existants sans écraser
	// 		ARulePaths &pathsInMap = mapPath[rule];
	// 		pathsInMap.insert(pathsInMap.end(), cachedPaths.begin(), cachedPaths.end());
	// 	}
	// 	return;
	// }

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

bool Parser::word(TerminalCharInfo	&data,
				  const char		 c,
				  const ARule *const rule,
				  const ARulePath	&path,
				  const ARuleMap	&mapPathCurrent,
				  ARuleMap			&mapPathNext)
{
	// Si on a déjà dépassé la fin du terminal
	if (data.position >= data.length)
		return false;

	// const char *value = data.terminal->getValue().c_str();

	// Match du caractère courant
	if (data.terminal->getValue()[data.position] != c)
		return false;

	// Si c’est le dernier caractère du terminal
	if (data.position == data.length - 1)
	{
		addNextPaths(mapPathCurrent, mapPathNext, path, rule);
		return true;
	}

	// Sinon, on avance dans le terminal
	++data.position;

	// On récupère le chemin correspondant dans mapPathNext
	ARulePaths &paths	  = mapPathNext[rule];
	size_t		pathCount = paths.size();

	// Parcours pour voir si le path existe déjà
	for (size_t i = 0; i < pathCount; ++i)
	{
		ARulePathInfo &it = paths[i];
		if (it.path == path)
		{
			it.terminalCharsInfo.push_back(data); // push_back au lieu de push
			return true;
		}
	}

	// Sinon on crée un nouveau path
	ARulePathInfo pathInfoObj(path);
	pathInfoObj.terminalCharsInfo.reserve(1); // pré-réserver 1 élément pour éviter resize
	pathInfoObj.terminalCharsInfo.push_back(data);
	paths.push_back(pathInfoObj);

	return true;
}


bool Parser::parseCurrentChar(const char	  c,
							  const ARuleMap &mapPathCurrent,
							  ARuleMap		 &mapPathNext)
{
	bool isValidWithCurrentPath = false;

	// Parcours des règles actuelles
	for (ARuleMap::const_iterator it = mapPathCurrent.begin();
		 it != mapPathCurrent.end(); ++it)
	{
		const ARule				  *rule	 = it->first;
		const ARulePaths		  &paths = it->second;
		const Vec<TerminalSymbol> &items = rule->getItems();

		size_t pathsCount = paths.size();
		size_t itemsCount = items.size();

		// Parcours de tous les chemins pour cette règle
		for (size_t pathIndex = 0; pathIndex < pathsCount; ++pathIndex)
		{
			size_t indexElementTerminalChar = 0;

			for (size_t itemIndex = 0; itemIndex < itemsCount; ++itemIndex)
			{
				const TerminalSymbol &item = items[itemIndex];

				if (!item.isTerminalChar())
					continue;

				// Crée un TerminalCharInfo local
				TerminalCharInfo data;
				if (paths[pathIndex].terminalCharsInfo.has(indexElementTerminalChar))
				{
					data = paths[pathIndex].terminalCharsInfo[indexElementTerminalChar];
				}
				else
				{
					data = TerminalCharInfo(&item, 0);
				}

				// Appel à word() avec la variable locale, position pourra être modifiée
				if (word(data, c, rule, paths[pathIndex].path, mapPathCurrent, mapPathNext))
				{
					isValidWithCurrentPath = true;
				}

				++indexElementTerminalChar;
			}
		}
	}

	return isValidWithCurrentPath;
}

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

std::ostream &operator<<(std::ostream &os, const BNFPath &data)
{
	os << data.next.size();
	return os;
}

// Fonction helper pour remplir le cache à l'initialisation
void Parser::precomputeOptional(const ARule *rule)
{
	// Si déjà calculé, on ne fait rien
	if (m_optionalCache.find(rule) != m_optionalCache.end())
		return;

	// Cycle detection pour les règles récursives
	if (m_optionalComputing.find(rule) != m_optionalComputing.end())
	{
		m_optionalCache[rule] = false; // cycle → non optionnel
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
		result							 = true; // on suppose que tous sont optionnels
		for (size_t i = 0; i < items.size(); ++i)
		{
			const TerminalSymbol &item = items[i];

			if (item.isTerminalChar())
			{
				result = false; // terminal obligatoire
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
		if (isValidEmptyFile())
			Logger::log("Parsing complete ✅");
		else
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

std::ostream &operator<<(std::ostream &os, const PathCacheKey &key)
{
	os << "PathCacheKey(conceptName: " << key.conceptName << ", pathContext size: " << LoggerJson::get(key.pathContext, true) << ")";
	return os;
}