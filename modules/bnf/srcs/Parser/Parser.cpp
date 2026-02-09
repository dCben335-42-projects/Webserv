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


// struct JsonMapKey
// {
// 		size_t	   index;
// 		ARulePath path;
// };

bool same_prec(ARulePath path, ARulePath pathNext, size_t n)
{
	// Logger::log("compare : " + LoggerJson::get(path) + " with " + LoggerJson::get(pathNext) + " with n = " + LoggerJson::get(n));
	if (path.size() < n || pathNext.size() < n)
		return false;

	for (size_t i = 0; i < n; ++i)
	{
		// Logger::log("compare : " + LoggerJson::get(path[i]) + " with " + LoggerJson::get(pathNext[i]));
		if (path[i] != pathNext[i])
			return false;
	}

	return true;
}


void recurseJson(
	ARulePath				path,
	Vec<ARulePath>			pathNexts,
	size_t					index_rule,
	AJson				  **json,
	size_t				   *index,
	Vec<std::string>	   &line,
	std::string			   &buffer,
	Map<size_t, AJson *>   &mapIndexToJson,
	Vec<const ARule *>	   &collectStr,
	bool					hasCollectStr,
	bool				   &continueBuffer,
	Map<size_t, ARulePath> &pathStack)
{

	AJson		*ptr  = NULL;
	const ARule *rule = path[index_rule];


	Logger::log("parent_cpt: " + LoggerJson::get(rule->getConceptName()));
	// Logger::log("parent_cpt: " + LoggerJson::get(rule));
	Logger::log("hasCollectStr: " + LoggerJson::get(hasCollectStr));

	bool tempHasCollectStr = hasCollectStr;
	(void) tempHasCollectStr;



	if (!hasCollectStr && rule->getConceptPtr()->isTerminalConcept())
	{
		collectStr	  = path.slice(index_rule);
		hasCollectStr = true;
		Logger::log("Ok8888");
	}

	if (path.size() - 1 == index_rule)
	{
		size_t lineSize = line[0].size();
		buffer.append(line[0]);
		line.remove(0);
		(*index) += lineSize;
		Logger::log(buffer);

		if (*index < pathNexts.size() && ((hasCollectStr && !path[index_rule - 1]->isSequence() && same_prec(path, pathNexts[*index], index_rule)) || same_prec(path, pathNexts[*index], index_rule + 1)))
		{
			continueBuffer = true;
			Logger::log("Ok333");
		}
		if (*index < pathNexts.size() && !same_prec(path, pathNexts[*index], index_rule))
		{
			Logger::log("Ok4");
			if (!hasCollectStr)
			{
				Logger::log("Ok18");

				*json = AJson::createString(buffer);
				buffer.clear();
			}
		}
		if (index_rule == 0)
		{
			Logger::log("Ok555");
			*json = AJson::createString(buffer);
			buffer.clear();
		}
		return;
	}

	Logger::log("OkSKIP");


	recurseJson(path, pathNexts, index_rule + 1, &ptr, index, line, buffer, mapIndexToJson, collectStr, hasCollectStr, continueBuffer, pathStack);
	Logger::log("----<UP>----");

	if (!ptr)
	{

		if (hasCollectStr && !continueBuffer)
		{
			if (*index < pathNexts.size() && same_prec(path, pathNexts[*index], index_rule))
			{
				continueBuffer = true;
			}
		}

		if (!continueBuffer && !hasCollectStr)
		{
			Logger::log("->1");
			if (index_rule + 1 < path.size())
			{
				Logger::log("->2");
				Logger::log("concept: " + LoggerJson::get(path[index_rule + 1]->getConceptName()));

				if (!path[index_rule + 1]->getConceptPtr()->isTerminalConcept())
				{
					Logger::log("->3");

					if (pathStack.has(index_rule + 1))
					{
						if (pathStack.get(index_rule + 1) != path.slice(index_rule + 1))
						{
							Logger::log("get");
							AJson *currentJson2 = mapIndexToJson.get(index_rule + 1);
							mapIndexToJson.get(index_rule)->asArray()->push(currentJson2);
							mapIndexToJson.remove(index_rule + 1);
						}
					}

					const std::string key = path[index_rule + 1]->getConceptName();
					if (!mapIndexToJson.has(index_rule + 1))
					{
						Logger::log("create_object");

						pathStack.set(index_rule + 1, path.slice(index_rule + 1));
						mapIndexToJson.set(index_rule + 1, AJson::createObject());
						mapIndexToJson.get(index_rule + 1)->asObject()->set(key, AJson::createArray());

						Logger::log("Object created: " + LoggerJson::get(mapIndexToJson.get(index_rule + 1)));
					}

					if (!buffer.empty())
					{
						if (path[index_rule + 1]->isSequence())
						{
							AJson	  *currentJson	 = mapIndexToJson.get(index_rule + 1);
							JsonArray *currentObject = currentJson->asObject()->get(path[index_rule + 1]->getConceptName())->asArray();
							AJson	  *lastElement	 = currentObject->getLast();
							if (!lastElement || lastElement->getType() != AJson::ARRAY)
								currentObject->push(AJson::createArray());
							currentObject->getLast()->asArray()->push(AJson::createString(buffer));
						}
						else
						{
							mapIndexToJson.get(index_rule + 1)->asObject()->get(key)->asArray()->push(AJson::createString(buffer));

							if (path[index_rule]->isSequence() && path[index_rule + 1]->isEnd())
							{
								const std::string key2			= path[index_rule]->getConceptName();
								JsonArray		 *currentObject = mapIndexToJson.get(index_rule)->asObject()->get(key2)->asArray();

								AJson *lastElement = currentObject->getLast();
								if (!lastElement || lastElement->getType() != AJson::ARRAY)
								{
									currentObject->push(AJson::createArray());
									lastElement = currentObject->getLast()->asArray();
								}
								lastElement->asArray()->push(mapIndexToJson.get(index_rule + 1));
								mapIndexToJson.remove(index_rule + 1);
								pathStack.remove(index_rule + 1);
							}
						}
						buffer.clear();
					}

					if (*index >= pathNexts.size() || (!same_prec(path, pathNexts[*index], index_rule + 1)))
					{
						Logger::log("Ok56");
						AJson *currentJson2 = mapIndexToJson.get(index_rule + 1);
						if (index_rule == 0)
							mapIndexToJson.get(index_rule)->asArray()->push(currentJson2);
						else
						{
							mapIndexToJson.get(index_rule)->asObject()->get(path[index_rule]->getConceptName())->asArray()->push(currentJson2);
						}
						Logger::log("Ok50");
						mapIndexToJson.remove(index_rule + 1);
						pathStack.remove(index_rule + 1);
					}

					Logger::log("Ok97");


					return;
				}
			}
		}

		// if (!hasCollectStr && !rule->getConceptPtr()->isTerminalConcept())
		// {
		// 	*json = AJson::createObject();
		// }

		if (!continueBuffer && index_rule == 0)
		{
			Logger::log("Ok99995");
			*json = AJson::createString(buffer);
			return;
		}
		Logger::log("Ok59");

		return;
	}
	else
	{
		Logger::log("Ok6");

		Logger::log("string : " + LoggerJson::get(buffer));
		// Si TerminalConcept on doit ajouter des string au create string
		if (path[index_rule]->isSequence())
		{
			if (!mapIndexToJson.has(index_rule))
				mapIndexToJson.set(index_rule, AJson::createArray());
		}
		else
		{
			if (!mapIndexToJson.has(index_rule))
				mapIndexToJson.set(index_rule, AJson::createArray());
		}

		AJson *currentJson = mapIndexToJson.get(index_rule);
		if (currentJson->getType() == AJson::ARRAY)
		{
			currentJson->asArray()->push(ptr);
		}
		if (!same_prec(path, pathNexts[*index], index_rule - 1))
		{
			*json = currentJson;
			return;
		}
	}
}

void generateJson(const BNFPath *cursor, const std::string &stringToParsed)
{
	// clang-format off

	// START VARS

	// ARulePath path = cursor->rulePath;
	// JsonArray *line	 = AJson::createArray();
	// size_t index = 0;
	// AJson *ptr = NULL;

	Vec<ARulePath> pathStack;
	pathStack.push_back(cursor->rulePath);

	size_t inc = 0;
	while (cursor && cursor->next.size() > 0)
	{
		for (size_t i = 0; i < cursor->next.size(); ++i)
		{
			if (!cursor->next[i].isInvalidedPath)
			{
				cursor = &cursor->next[i];
				inc++;
				break;
			}
		}
		pathStack.push_back(cursor->rulePath);
	}

	// [....][.....]
	// [..][..][...]
	// Logger::log(LoggerJson::get(pathStack, true));

	Vec<std::string> line;
	std::string str = "";

	if (!pathStack.empty())
	{
		str.push_back(stringToParsed[0]);  // char

		for (size_t i = 1; i < pathStack.size(); ++i)
		{
			if (pathStack[i - 1] == pathStack[i])
			{
				str.push_back(stringToParsed[i]);  // char
			}
			else
			{
				line.push_back(str);
				Logger::log(LoggerJson::get(str));
				str = "";
				str.push_back(stringToParsed[i]);
			}
		}

		line.push_back(str);
	}

	Vec<AJson *> elements;
	AJson *json = NULL;
	std::string buffer;
	size_t index = 0;


	Map<size_t, AJson *> mapIndexToJson;
	bool continueBuffer = false;
	Vec<const ARule *> collectStr;

	Map<size_t, ARulePath> pathsAll;

	AJson *arr = AJson::createArray();
	mapIndexToJson.set(0, arr);

	while (index < pathStack.size())
	{
		Logger::log("=======");
		recurseJson(pathStack[index], pathStack, 0, &json, &index, line, buffer, mapIndexToJson, collectStr, false, continueBuffer, pathsAll);
		Logger::log("json : " + LoggerJson::get(json));
		if (!continueBuffer)
		{
			buffer.clear();
		}
		continueBuffer = false;
		if (json)
		{
			mapIndexToJson.get(0)->asArray()->push(json);
			json = NULL;
		}
		if (mapIndexToJson.has(0))
			Logger::log("arr : " + LoggerJson::get(mapIndexToJson.get(0), false));
	}
	// Logger::log(line2);
}




void Parser::resolvePath(const std::string &stringToParsed)
{
	// clang-format off
	Vec<BNFPath *> basePaths;
	Vec<BNFPath *> pathsTop;
	Map<ARulePath, Vec<ARulePath> > *mapPrev = &m_indexedPaths[0];
	typedef Map<ARulePath, Vec<ARulePath> >::const_iterator MapIt;
	for (MapIt it = mapPrev->begin(); it != mapPrev->end(); ++it)
	{
		basePaths.push_once(new BNFPath(it->first, 0));
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
							pathsTop[l]->setNext(vecKeyValue, k);
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
	
	(void) stringToParsed;
	const BNFPath *nextbnf = pathsTop[0]->getHead();
	generateJson(nextbnf, stringToParsed);
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
	getPathsTerminalChars(ARulePath(), CURSOR, mapPathCurrent, Vec<std::string>(), ARulePath());

	for (size_t i = 0; i < stringToParsed.size(); ++i)
	{
		c = stringToParsed[i];
		if (!parseCurrentChar(c, mapPathCurrent, mapPathNext))
			throw InvalidEntryException(std::string("Invalid character '") + c);
		completePath();
		if (i == stringToParsed.size() - 1)
			break;

		mapPathCurrent = mapPathNext;
		mapPathNext.clear();
	}
	resolvePath(stringToParsed);


	if (mapPathNext.empty() || verifyOptionalEnd(mapPathCurrent, c)) // Pas tout a fait correct si il existe une suite pleinement optionnele cest aussi valide
		Logger::log("Parsing complete ✅");
	else
		throw FileNotFinishedException("END: The file ended before reaching a valid end state.");
}