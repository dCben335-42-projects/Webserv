#include <sstream>
#include <iostream>
#include <algorithm>
#include "./Parser.hpp"
#include "../ARulePathInfo/ARulePathInfo.hpp"
#include "Json.hpp"
#include "../BNFPath/BNFPath.hpp"
#include "../BNFSignals/BNFSignals.hpp"

/**
 * @brief Constructs a Parser object with the given SemanticDescriptor.
 * @param semantic The SemanticDescriptor that defines the grammar rules and concepts for parsing.
 */
Parser::Parser(const SemanticDescriptor &semantic) : m_semantic(semantic)
{
}

/**
 * @brief Destructor for the Parser class.
 */
Parser::~Parser()
{
}

/**
 * @brief Adds a path possibility to the ARuleMap based on the given rule and concept name.
 * @param mapPath The ARuleMap to which the path possibility will be added.
 * @param rule The ARule for which the path possibility is being added.
 * @param completePath The current complete path of ARules being explored.
 */
void Parser::addPathPossibility(
	ARulePath		  &completePath,
	const ARule *const rule,
	ARuleMap		  &mapPath)
{
	const Vec<TerminalSymbol> &items = rule->getItems();
	const size_t			   size	 = items.size();

	for (size_t i = 0; i < size; ++i)
	{
		const TerminalSymbol &item		= items[i];
		const std::string	 *itemValue = item.getValuePtr();
		if (m_conceptPaths.contains(itemValue))
			continue;
		if (!item.isTerminalChar())
			getPathsTerminalChars(completePath, itemValue, mapPath);
		else
			createPathInfo(mapPath[rule], completePath);
	}
}

/**
 * @brief Creates an ARulePathInfo object for the given complete path and adds it to the ARuleMap if it doesn't already exist.
 * @param element The ARulePaths vector to which the ARulePathInfo will be added if it doesn't already exist.
 * @param completePath The complete path of ARules for which the ARulePathInfo is being created.
 */
void Parser::createPathInfo(
	ARulePaths		&element,
	const ARulePath &completePath)
{
	size_t pathHash = ARulePathInfo::createPathHash(completePath);
	for (ARulePaths::iterator it = element.begin(); it != element.end(); ++it)
		if ((*it)->m_pathHash == pathHash)
			return;
	element.push_back(createARulePathInfo(completePath, pathHash));
}

/**
 * @brief Creates an ARulePathInfo object for the given path and path hash. If an ARulePathInfo with the same path hash already exists and is not replaceable, it resets its terminal characters info and returns it. Otherwise, it creates a new ARulePathInfo, adds it to the list, and returns it.
 * @param path The ARulePath for which the ARulePathInfo is being created.
 * @param pathHash The hash of the ARulePath used for quick lookup.
 * @return A pointer to the created or existing ARulePathInfo object.
 */
ARulePathInfo *Parser::createARulePathInfo(const ARulePath &path, size_t pathHash)
{
	size_t n = m_arulePathInfoList.size();
	for (size_t i = 0; i < n; ++i)
	{
		ARulePathInfo &an = m_arulePathInfoList[i];
		if (an.m_pathHash == pathHash && an.m_replaceable == 0)
		{
			an.resetTerminalCharsInfo();
			return &an;
		}
	}
	if (m_arulePathInfoList.size() >= BNF_CACHE_SIZE)
		throw CacheFullException();
	m_arulePathInfoList.push_back(ARulePathInfo(path));
	return &m_arulePathInfoList.back();
}

/**
 * @brief Adds sequence paths to the ARuleMap based on the given path.
 * @param path The ARulePath representing the current sequence of rules.
 * @param mapPathNext The ARuleMap to which the sequence paths will be added.
 */
void Parser::addSequencePaths(const ARulePath &path, ARuleMap &mapPathNext)
{
	int lastIndex = path.size() - 1;
	for (int i = lastIndex; i >= 0; --i)
	{
		const ARule *const rule = path[i];
		if (rule->is(SEQUENCE))
		{
			ARulePath subPath = path;
			if (i != lastIndex)
				subPath.resize(i + 1);
			processTerminalSymbol(rule, subPath, mapPathNext);
		}

		const ARule *adjacentRule = rule->tryGetAdjacentRules();
		while (adjacentRule && isFullyOptional(*adjacentRule))
			adjacentRule = adjacentRule->tryGetAdjacentRules();
		if (adjacentRule)
			return;
	}
}

/**
 * @brief Processes a terminal symbol and updates the ARuleMap with the corresponding paths.
 * @param rule The ARule associated with the terminal symbol.
 * @param path The current ARulePath being explored.
 * @param mapPath The ARuleMap to which the paths will be added based on the terminal symbol.
 */
void Parser::processTerminalSymbol(
	const ARule *rule,
	ARulePath	&path,
	ARuleMap	&mapPath)
{
	const Vec<TerminalSymbol> &items = rule->getItems();
	size_t					   n	 = items.size();
	for (size_t i = 0; i < n; ++i)
	{
		if (items[i].isTerminalChar())
			createPathInfo(mapPath[rule], path);
		else
			getPathsTerminalChars(path, items[i].getValuePtr(), mapPath);
	}
}

/**
 * @brief Retrieves the terminal characters associated with the given concept name and updates the ARuleMap accordingly.
 * @param completePath The current complete path of ARules being explored.
 * @param conceptName The name of the concept for which terminal characters are being retrieved.
 * @param mapPath The ARuleMap to which the terminal character paths will be added.
 */
void Parser::getPathsTerminalChars(ARulePath &completePath, const std::string *conceptName, ARuleMap &mapPath)
{
	m_conceptPaths.push_back(conceptName);
	const Vec<ConceptDef *>			  &defs			   = m_semantic.getConceptDefs(*conceptName);
	const Set<const ARule *>		  &terminalChars   = m_semantic.getConcept(*conceptName)->getTerminalChars();
	Set<const ARule *>::const_iterator ite			   = terminalChars.end();
	size_t							   conceptDefCount = defs.size();

	for (size_t i = 0; i < conceptDefCount; ++i)
	{
		const ConceptDef *def  = defs[i];
		size_t			  size = def->size();
		size_t			  k	   = 0;
		while (k < size)
		{
			const ARule *const subRule = def->getARuleUnsafe(k);
			completePath.push_back(subRule);
			addPathPossibility(completePath, subRule, mapPath);
			if (terminalChars.find(subRule) != ite)
				createPathInfo(mapPath[subRule], completePath);
			completePath.pop_back();
			if (!isFullyOptional(*subRule))
				break;
			++k;
		}
	}
	m_conceptPaths.pop_back();
}

/**
 * @brief Adds the next paths to the ARuleMap based on the current path and rule.
 * @param mapPathCurrent The current ARuleMap representing the active paths.
 * @param mapPathNext The ARuleMap to which the next paths will be added.
 * @param path The current ARulePath being explored.
 * @param rule The ARule for which the next paths are being added.
 */
void Parser::addNextPaths(const ARuleMap &mapPathCurrent, ARuleMap &mapPathNext, const ARulePath &path, const ARule *const rule)
{
	addSequencePaths(path, mapPathNext);
	ARulePath nextRule = rule->getNextRules(path);
	if (nextRule.empty())
		return;
	const ARule *const lastRule = nextRule.back();
	if (isFullyOptional(*lastRule))
		addNextPaths(mapPathCurrent, mapPathNext, nextRule, lastRule);
	processTerminalSymbol(lastRule, nextRule, mapPathNext);
}

/**
 * @brief Processes a terminal character in the parsing process and updates the ARuleMap accordingly.
 * @param data The TerminalCharInfo containing information about the current terminal character being processed.
 * @param c The current character being parsed from the input string.
 * @param rule The ARule associated with the current path being processed.
 * @param path The current ARulePath being explored.
 * @param mapPathCurrent The current ARuleMap representing the active paths before processing the character.
 * @param mapPathNext The ARuleMap to which the next paths will be added after processing the character.
 * @return true if the character is valid for the current path and rule, false otherwise.
 */
bool Parser::word(
	const char		   c,
	const ARule *const rule,
	ARulePathInfo	  *pathInfo,
	const ARuleMap	  &mapPathCurrent,
	ARuleMap		  &mapPathNext,
	size_t			   indexElementTerminalChar)
{
	TerminalCharInfo &data = pathInfo->terminalCharsInfo[indexElementTerminalChar];

	if (data.position >= data.length || pathInfo->getValue()[indexElementTerminalChar][data.position] != c)
	{
		pathInfo->m_replaceable -= 1;
		return false;
	}

	if (data.position++ == data.length - 1)
	{
		pathInfo->m_replaceable -= 1;
		addNextPaths(mapPathCurrent, mapPathNext, pathInfo->m_path, rule);
		return true;
	}

	ARulePaths &paths	  = mapPathNext[rule];
	size_t		pathCount = paths.size();

	for (size_t i = 0; i < pathCount; ++i)
	{
		ARulePathInfo *it = paths[i];
		if (it->m_path == pathInfo->m_path)
		{
			it->terminalCharsInfo[indexElementTerminalChar].position = data.position;
			return true;
		}
	}

	paths.push_back(pathInfo);
	return true;
}

/**
 * @brief Parses the current character against the active paths in the ARuleMap and updates the next paths accordingly.
 * @param c The current character being parsed from the input string.
 * @param mapPathCurrent The current ARuleMap representing the active paths before processing the character.
 * @param mapPathNext The ARuleMap to which the next paths will be added after processing the character.
 * @return true if the character is valid for at least one of the current paths and rules, false otherwise.
 */
bool Parser::parseCurrentChar(
	const char		c,
	const ARuleMap &mapPathCurrent,
	ARuleMap	   &mapPathNext)
{
	bool isValidWithCurrentPath = false;

	for (ARuleMap::const_iterator it = mapPathCurrent.begin();
		 it != mapPathCurrent.end(); ++it)
	{
		const ARule				  *rule		  = it->first;
		const ARulePaths		  &paths	  = it->second;
		const Vec<TerminalSymbol> &items	  = rule->getItems();
		size_t					   pathsCount = paths.size();
		size_t					   itemsCount = items.size();

		for (size_t pathIndex = 0; pathIndex < pathsCount; ++pathIndex)
		{
			size_t indexElementTerminalChar = 0;

			for (size_t itemIndex = 0; itemIndex < itemsCount; ++itemIndex)
			{
				if (!items[itemIndex].isTerminalChar())
					continue;

				if (word(c, rule, paths[pathIndex], mapPathCurrent, mapPathNext, indexElementTerminalChar))
					isValidWithCurrentPath = true;
				++indexElementTerminalChar;
			}
		}
	}
	return isValidWithCurrentPath;
}

/**
 * @brief Checks if the given ARule is fully optional.
 * @param rule The ARule to be checked.
 * @return true if the ARule is fully optional, false otherwise.
 */
bool Parser::isFullyOptional(const ARule &rule)
{
	std::map<const ARule *, bool>::iterator cacheIt = m_optionalCache.find(&rule);
	if (cacheIt != m_optionalCache.end())
		return cacheIt->second;

	if (m_optionalComputing.find(&rule) != m_optionalComputing.end())
		return false;

	m_optionalComputing.insert(&rule);

	bool result = false;

	if (rule.is(OPTION))
		result = true;
	else
	{
		const Vec<TerminalSymbol> &items = rule.getItems();
		const size_t			   n	 = items.size();

		for (size_t i = 0; i < n; ++i)
		{
			if (items[i].isTerminalChar())
				continue;

			const Vec<ConceptDef *> &defs  = m_semantic.getConceptDefs(items[i].getValue());
			const size_t			 j_max = defs.size();

			for (size_t j = 0; j < j_max; ++j)
			{
				const ARulePath &rules		= defs[j]->getRules();
				const size_t	 rulesCount = rules.size();

				bool allOptional = true;

				for (size_t k = 0; k < rulesCount; ++k)
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

/**
 * @brief Checks if the given ARulePath is fully optional until the end.
 * @param path The ARulePath to be checked.
 * @return true if the ARulePath is fully optional until the end, false otherwise.
 */
bool Parser::isFullyOptionalEnd(const ARulePath &path)
{
	ARulePath current = path;
	while (!current.empty())
	{
		const ARule *rule = current.back();
		if (!isFullyOptional(*rule))
			return false;
		current = rule->getNextRules(current);
	}
	return true;
}

/**
 * @brief Checks if the given ARulePath is fully optional until the end, starting from the next rules of the last ARule in the path.
 * @param path The ARulePath to be checked.
 * @return true if the ARulePath is fully optional until the end, false otherwise.
 */
bool Parser::isFullyOptionalEndAdvance(const ARulePath &path)
{
	return isFullyOptionalEnd(path.back()->getNextRules(path));
}

/**
 * @brief Verifies if the current ARuleMap can lead to a valid end state with the given character.
 * @param mapPathCurrent The current ARuleMap representing the active paths before processing the character.
 * @param c The current character being parsed from the input string.
 * @return true if the current ARuleMap can lead to a valid end state with the given character, false otherwise.
 */
bool Parser::verifyOptionalEnd(const ARuleMap &mapPathCurrent, const char c)
{
	for (ARuleMap::const_iterator it = mapPathCurrent.begin(); it != mapPathCurrent.end(); ++it)
	{
		const ARulePaths &val = it->second;
		for (ARulePaths::const_iterator path = val.begin(); path != val.end(); ++path)
		{
			const ARulePath			  &rules = (*path)->m_path;
			const Vec<TerminalSymbol> &items = rules.back()->getItems();
			for (Vec<TerminalSymbol>::const_iterator item = items.begin(); item != items.end(); ++item)
				if (item->isTerminalChar() && *(item->getValue().end() - 1) == c && isFullyOptionalEndAdvance(rules))
					return true;
		}
	}
	return false;
}


/**
 * @brief Checks if the file can be considered valid when it is empty, based on the grammar rules defined in the SemanticDescriptor.
 * @return true if the file can be considered valid when empty, false otherwise.
 */
bool Parser::isValidEmptyFile()
{
	ARulePath				 startPath;
	const Vec<ConceptDef *> &defs	   = m_semantic.getConceptDefs(CURSOR);
	const size_t			 defsCount = defs.size();
	for (size_t i = 0; i < defsCount; ++i)
	{
		startPath.clear();
		startPath.push_back(defs[i]->getARuleUnsafe(0));
		if (isFullyOptionalEnd(startPath))
			return true;
	}
	return false;
}

/**
 * @brief Parses the given string according to the grammar rules defined in the SemanticDescriptor.
 * @param stringToParsed The string to be parsed.
 */
void Parser::parse(const std::string &stringToParsed)
{
	if (stringToParsed.empty())
	{
		if (!isValidEmptyFile())
			throw FileNotFinishedException("The file must not be empty.");
		return;
	}

	ARuleMap  mapPathCurrent;
	ARuleMap  mapPathNext;
	ARulePath completePath;
	char	  c;

	m_arulePathInfoList.reserve(BNF_CACHE_SIZE);
	m_conceptPaths.reserve(8);
	completePath.reserve(8);

	getPathsTerminalChars(completePath, &CURSOR, mapPathCurrent);
	size_t size		 = stringToParsed.size();
	size_t lastIndex = size - 1;

	for (size_t i = 0; i < size; ++i)
	{
		if (BNFSignals::getEnd())
			throw FileNotFinishedException("Parsing was interrupted by a termination signal.");
		c = stringToParsed[i];
		if (!parseCurrentChar(c, mapPathCurrent, mapPathNext))
			throwInvalidEntryException(mapPathCurrent, c, i);
		if (i == lastIndex)
			break;

		mapPathCurrent = mapPathNext;
		mapPathNext.clear();
	}
	Logger::log("ARulePathInfoList size: " + Logger::get(m_arulePathInfoList.size()));

	if (mapPathNext.empty() || verifyOptionalEnd(mapPathCurrent, c))
		Logger::log("Parsing complete ✅");
	else
		throw FileNotFinishedException("END: The file ended before reaching a valid end state.");
}


void Parser::throwInvalidEntryException(const ARuleMap &mapPathCurrent, const char c, size_t i) const
{
	std::string expectedChars;
	for (ARuleMap::const_iterator it = mapPathCurrent.begin(); it != mapPathCurrent.end(); ++it)
	{
		const ARulePaths &val = it->second;
		for (ARulePaths::const_iterator paths = val.begin(); paths != val.end(); ++paths)
		{
			ARulePathInfo *pathInfo = *paths;
			for (size_t j = 0; j < pathInfo->m_path.size(); ++j)
			{
				expectedChars += pathInfo->m_path[j]->getConceptName();
				expectedChars += " -> ";
			}
			for (size_t k = 0; k < pathInfo->getValue().size(); ++k)
			{
				expectedChars += "'" + pathInfo->getValue()[k] + "'";
				if (k != pathInfo->getValue().size() - 1)
					expectedChars += ", ";
				else
					expectedChars += " ";
			}
			if (paths + 1 != val.end())
				expectedChars += "\n";
		}
		expectedChars += "\n";
	}
	throw InvalidEntryException(std::string("Invalid character '") + c + "' at position " + Logger::get(i + 1) + ".\nExpected characters:\n" + expectedChars);
}