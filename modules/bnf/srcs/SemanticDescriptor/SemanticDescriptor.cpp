#include <sstream>
#include <iostream>
#include "./SemanticDescriptor.hpp"
#include "Vector.hpp"
#include "Logger.hpp"
#include "../utils/utils.hpp"

using namespace bnf;

SemanticDescriptor::SemanticDescriptor() : tree(NULL)
{
}

SemanticDescriptor::SemanticDescriptor(const std::string &m_content)
{
	std::istringstream iss(m_content);
	splitBlocks(iss);
	parseBlocks();
}

SemanticDescriptor::~SemanticDescriptor()
{
	delete tree;

	for (Map<Concept *>::iterator it = m_concepts.begin(); it != m_concepts.end(); ++it)
	{
		Vec<ConceptDef *> defs = (*it).second->getDefinitions();
		for (Vec<ConceptDef *>::iterator conceptIt = defs.begin(); conceptIt != defs.end(); ++conceptIt)
		{
			delete *conceptIt;
		}
		delete (*it).second;
	}
}

bool SemanticDescriptor::isStartRule(const std::string &line, size_t *startDef)
{
	size_t size = line.size();
	size_t i	= 0;

	while (i < size && std::isspace(line[i]))
		++i;
	if (i + 2 <= size && line.compare(i, 2, "|=") == 0)
	{
		*startDef = i + 2;
		return true;
	}
	size_t start = i;
	while (i < size && (std::isalnum(line[i]) || line[i] == '_'))
		++i;
	if (i == start)
		return false;
	while (i < size && std::isspace(line[i]))
		++i;
	if (i + 3 <= size && line.compare(i, 3, "::=") == 0)
	{
		*startDef = i + 3;
		return true;
	}
	return false;
}

void SemanticDescriptor::splitBlocks(std::istringstream &iss)
{
	std::string line, cblock;
	size_t		startDef = 0, prevStartDef = 0;

	while (std::getline(iss, line))
	{
		if (isStartRule(line, &startDef) && !cblock.empty())
		{
			if (isOnlyWhitespace(cblock, prevStartDef))
				throw ConceptHasNotDefinitionException();
			blocks.push_back(cblock);
			cblock.clear();
			prevStartDef = startDef;
		}
		cblock += line + '\n';
	}
	if (!cblock.empty())
		blocks.push_back(cblock);
}

bool SemanticDescriptor::findSeparator(const std::string &block, size_t &pos)
{
	if ((pos = block.find(EQRULE)) != std::string::npos)
	{
		return true;
	}

	if ((pos = block.find(ORRULE)) != std::string::npos)
	{
		pos += ORRULE.length();
		return false;
	}

	throw ConceptHasNotDefinitionException();
}

void SemanticDescriptor::parseBlocks()
{
	size_t pos;

	try
	{
		for (Vec<std::string>::iterator block = blocks.begin(); block != blocks.end(); ++block)
		{
			if (findSeparator(*block, pos))
			{
				parseConceptName(block->substr(0, pos));
				pos += EQRULE.length();
			}
			parseRule(*block, pos);
		}
		checkValidity();
		addParent();
		findAllNext();
	}
	catch (const SemanticException &e)
	{
		std::cout << e;
	}
}


void SemanticDescriptor::parseRule(const std::string &m_content, size_t cursor)
{
	Concept *concept;
	ConceptDef *conceptDef = new ConceptDef(m_lastKey);

	if (!m_concepts.try_get(m_lastKey, concept))
	{
		concept = new Concept(m_lastKey);
		m_concepts.set(m_lastKey, concept);
	}

	size_t conceptDefIndex = concept->getDefinitions().size();
	concept->pushDef(conceptDef);

	std::istringstream iss(m_content.substr(cursor));
	std::string		   word;
	ARule			  *arule	  = NULL;
	size_t			   aruleIndex = 0;

	while (iss >> word)
	{
		if (ARule::isKeyWord(word, true))
			throw std::runtime_error("Invalid keyword: " + word);
		BnfMap map = ARule::getMap(word);
		arule	   = map.constructor(m_lastKey, conceptDefIndex, aruleIndex, concept);
		if (map.hasClosure && !(iss >> word))
			throw std::runtime_error("Unexpected end of rule after " + map.opener);
		while (word != map.closer)
		{
			if (ARule::isKeyWord(word, false))
				throw std::runtime_error("Bad closing keyword: " + word);
			if (word != DIVIDER)
			{
				if (!validate_string_strict(word))
					throw InvalidConceptNameException(word);
				arule->addItem(word);
				if (!map.hasClosure)
					break;
			}
			if (!(iss >> word))
				break;
		}
		conceptDef->setRule(arule);
		aruleIndex++;
	}
	conceptDef->finish();
}

void SemanticDescriptor::addParent()
{
	for (Map<Concept *>::iterator it = m_concepts.begin(); it != m_concepts.end(); ++it)
	{
		V_FOREACH(ConceptDef *, conceptIt, (*it).second->getDefinitions())
		{
			V_FOREACH(ARule *, ruleIt, (*conceptIt)->getValues())
			{
				Concept			*conceptPtr;
				ARule			*rule  = *ruleIt;
				Vec<std::string> items = rule->getItems();

				for (size_t i = 0; i < items.size(); ++i)
				{
					if (m_concepts.try_get(items[i], conceptPtr))
						conceptPtr->addParent(rule);
				}
			}
		}
	}
}

void SemanticDescriptor::ndw(ARule *rule, Vec<std::string> &conceptPaths)
{
	Concept			*conceptPtr;
	Vec<std::string> conceptPathsCopy;
	Vec<std::string> items = rule->getItems();
	for (size_t i = 0; i < items.size(); ++i)
	{
		if (conceptPaths.contains(items[i]))
			continue;
		if (m_concepts.try_get(items[i], conceptPtr))
		{
			conceptPathsCopy = conceptPaths;
			conceptPathsCopy.push_back(items[i]);
			V_FOREACH(ConceptDef *, conceptIt, conceptPtr->getDefinitions())
			{
				ndw((*conceptIt)->getARule(0), conceptPathsCopy);
			}
			continue;
		}
		for (size_t j = 0; j < conceptPaths.size(); ++j)
		{
			m_concepts.get(conceptPaths[j])->addTerminalChar(rule); // Peut etre faire en map  (key)char -> (value)set<ARule *>
		}
	}
}





void SemanticDescriptor::ndw2(ARule *rule, Vec<std::string> &conceptPaths, ARulePath path, Map<ARule *, ARulePaths> &mapPath, std::string conceptName) const
{
	Concept			*conceptPtr;
	Vec<std::string> conceptPathsCopy;
	Vec<std::string> items = rule->getItems();

	path.push_back(rule);

	for (size_t i = 0; i < items.size(); ++i)
	{
		if (conceptPaths.contains(items[i]))
			continue;
		if (m_concepts.try_get(items[i], conceptPtr))
		{
			conceptPathsCopy = conceptPaths;
			conceptPathsCopy.push_back(items[i]);
			V_FOREACH(ConceptDef *, conceptIt, conceptPtr->getDefinitions())
			{
				ndw2((*conceptIt)->getARule(0), conceptPathsCopy, path, mapPath, conceptName);
			}
			continue;
		}
	}

	Set<ARule *> terminalChars = m_concepts.get(conceptName)->getTerminalChars();
	for (Set<ARule *>::iterator it = terminalChars.begin(); it != terminalChars.end(); ++it)
	{
		if (rule == (*it))
		{
			if (!mapPath.has(rule))
			{
				mapPath.set(rule, ARulePaths());
			}
			mapPath.get(rule).insert(path);
		}
	}
}

void SemanticDescriptor::findAllNext()
{
	Vec<std::string> conceptPaths;
	ARulePath		 path;

	for (Map<Concept *>::iterator it = m_concepts.begin(); it != m_concepts.end(); ++it)
	{
		conceptPaths.push_back((*it).first);
		V_FOREACH(ConceptDef *, conceptIt, (*it).second->getDefinitions())
		{
			ndw((*conceptIt)->getARule(0), conceptPaths);
		}
		conceptPaths.clear();
	}

	m_startPath = getPathsTerminalChars(ARulePaths(), CURSOR);
	conceptPaths.clear();
	// printStartPath();
}

Map<ARule *, ARulePaths> SemanticDescriptor::getPathsTerminalChars(ARulePaths prefixs, std::string conceptName) const
{
	Vec<std::string>		 conceptPaths;
	Map<ARule *, ARulePaths> mapPath;
	ARulePath				 path;

	conceptPaths.push_back(conceptName);

	V_FOREACH(ConceptDef *, conceptIt, m_concepts.get(conceptName)->getDefinitions())
	{
		ndw2((*conceptIt)->getARule(0), conceptPaths, path, mapPath, conceptName);
	}
	if (prefixs.empty())
		return mapPath;

	Map<ARule *, ARulePaths> mapPathPrefixed;

	for (Map<ARule *, ARulePaths>::iterator it = mapPath.begin(); it != mapPath.end(); ++it)
	{
		mapPathPrefixed.set(it->first, ARulePaths());
	}

	for (Map<ARule *, ARulePaths>::iterator it = mapPath.begin(); it != mapPath.end(); ++it)
	{
		// Logger::log("Processing rule: " + LoggerJson::get(*(it->first), true));
		ARulePaths *paths = &mapPathPrefixed.get(it->first);
		for (ARulePaths::iterator x = prefixs.begin(); x != prefixs.end(); ++x)
		{


			ARulePath vec1 = *x;
			// Logger::log("vec1: " + LoggerJson::get(vec1, true));
			// Logger::log("it->second size: " + LoggerJson::get(it->second.size()));

			for (ARulePaths::iterator j = it->second.begin(); j != it->second.end(); ++j)
			{
				// Logger::log("\ndwid\n");
				ARulePath vec2 = *j;
				vec1.insert(vec1.end(), vec2.begin(), vec2.end());
				// Logger::log("vec1-2: " + LoggerJson::get(vec1, true));
				paths->insert(vec1);
			}
		}
	}
	return mapPathPrefixed;
}

// Map<ARule *, ARulePaths> getMapPath

// void SemanticDescriptor::printStartPath()
// {
// 	for (Map<ARule *, ARulePaths>::iterator it = m_startPath.begin(); it != m_startPath.end(); ++it)
// 	{
// 		Logger::log(LoggerJson::get(*(it->first), true) + "(" + LoggerJson::get(it->second.size()) + " paths) : ");
// 		for (size_t i = 0; i < it->second.size(); ++i)
// 		{
// 			Logger::log("[");
// 			for (size_t j = 0; j < it->second[i].size(); ++j)
// 			{
// 				Logger::log(LoggerJson::get(*(it->second[i][j]), true) + (j + 1 < it->second[i].size() ? ", " : ""));
// 			}
// 			Logger::log("]");
// 		}
// 	}
// }

Map<ARule *, ARulePaths> SemanticDescriptor::getStartPath() const
{
	return m_startPath;
}

void SemanticDescriptor::parseConceptName(std::string s)
{
	std::istringstream iss(erasePaternWrapper(s, WS));
	std::string		   w;
	if (!(iss >> w))
		throw EmptyConceptNameException();
	if (iss >> w)
		throw ConceptHasNotDefinitionException();
	if (isInMap(m_concepts, s))
		throw ConceptAlreadyDefinedException(s);
	m_lastKey = s;
}

bool SemanticDescriptor::hasRecursiveErrorOnNewNode(std::string item, ConceptNode *parentPtr)
{
	ConceptNode *tempN = new ConceptNode(item, m_concepts[item]->getDefinitions());
	tempN->setPath(parentPtr->getPath())
		.addSegmentPath(item)
		.setParent(parentPtr);

	return hasRecursiveError(*tempN);
}

bool SemanticDescriptor::hasRecursiveError(ConceptNode &node)
{
	Vec<std::string> items;
	unsigned int	 hasInfRecursivity;

	for (ConceptList::const_iterator it = node.getConcepts().begin(); it != node.getConcepts().end(); ++it)
	{
		items			  = (*it)->getItems();
		hasInfRecursivity = 0;

		for (Vec<std::string>::iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
		{
			const std::string &item = *itemIt;

			if (startsWith(item, DQSQ))
				continue;

			if (!isInMap(m_concepts, item))
				throw std::runtime_error(ERR_CONCEPT_NOT_FOUND + item);

			hasInfRecursivity += isInContainer(node.getPath(), item) || hasRecursiveErrorOnNewNode(item, &node);
		}
		if (!hasInfRecursivity)
			node.setHasInfRecursivity(false);
	}
	return node.hasInfRecursivity();
}

void SemanticDescriptor::checkValidity()
{
	tree = new ConceptNode(CURSOR, m_concepts[CURSOR]->getDefinitions());
	tree->addSegmentPath(CURSOR);
	if (hasRecursiveError(*tree))
		throw std::runtime_error("ERREUR !");
}

Map<Concept *> SemanticDescriptor::getConcepts() const
{
	return m_concepts;
}

Concept *&SemanticDescriptor::getConcept(const std::string &key)
{
	return m_concepts.get(key);
}

const Concept *SemanticDescriptor::getConcept(const std::string &key) const
{
	return m_concepts.get(key);
}

// const Concept *&SemanticDescriptor::getConcept(const std::string &key) const
// {
// 	return m_concepts.get(key);
// }


// bool SemanticDescriptor::tryGetConcept(const std::string &key, Concept &conceptList) const
// {
// 	return m_concepts.try_get(key, conceptList);
// }

ConceptNode *SemanticDescriptor::getTree() const
{
	return tree;
}

std::ostream &operator<<(std::ostream &os, const SemanticDescriptor &data)
{
	Logger::log(data.getConcepts());
	return (os);
}