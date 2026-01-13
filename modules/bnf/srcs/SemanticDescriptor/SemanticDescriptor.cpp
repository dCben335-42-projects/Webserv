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

	for (Map<ConceptList>::iterator it = m_concepts.begin(); it != m_concepts.end(); ++it)
	{
		for (Vec<ConceptDef *>::iterator conceptIt = it->second.begin(); conceptIt != it->second.end(); ++conceptIt)
		{
			delete *conceptIt;
		}
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
	}
	catch (const SemanticException &e)
	{
		std::cout << e;
	}
}


void SemanticDescriptor::parseRule(const std::string &m_content, size_t cursor)
{
	ConceptDef *concept	   = new ConceptDef(m_lastKey);
	size_t conceptDefIndex = m_concepts[m_lastKey].size();
	m_concepts[m_lastKey].push_back(concept);
	std::istringstream iss(m_content.substr(cursor));
	std::string		   word;
	ARule			  *arule	  = NULL;
	size_t			   aruleIndex = 0;

	while (iss >> word)
	{
		if (ARule::isKeyWord(word, true))
			throw std::runtime_error("Invalid keyword: " + word);
		BnfMap map = ARule::getMap(word);
		arule	   = map.constructor(m_lastKey, conceptDefIndex, aruleIndex);
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
		concept->setRule(arule);
		aruleIndex++;
	}
	concept->finish();
}

void SemanticDescriptor::addParent()
{
	for (Map<ConceptList>::iterator it = m_concepts.begin(); it != m_concepts.end(); ++it)
	{
		for (Vec<ConceptDef *>::iterator conceptIt = it->second.begin(); conceptIt != it->second.end(); ++conceptIt)
		{
			ConceptDef *concept = *conceptIt;
			Vec<ARule *> values = concept->getValues();

			for (Vec<ARule *>::iterator ruleIt = values.begin(); ruleIt != values.end(); ++ruleIt)
			{
				ARule			 *rule = *ruleIt;
				Vec<ConceptDef *> conceptDefs;
				Vec<std::string>  items = rule->getItems();

				if (rule->getType() == SYMBOL && m_concepts.try_get(items[0], conceptDefs))
				{
					for (Vec<ConceptDef *>::iterator childIt = conceptDefs.begin(); childIt != conceptDefs.end(); ++childIt)
					{
						ConceptDef *childConcept = *childIt;
						childConcept->addParent(rule);
					}
				}
				else
				{
					for (size_t i = 0; i < items.size(); ++i)
					{
						if (m_concepts.try_get(items[i], conceptDefs))
						{
							for (Vec<ConceptDef *>::iterator childIt = conceptDefs.begin(); childIt != conceptDefs.end(); ++childIt)
							{
								ConceptDef *childConcept = *childIt;
								childConcept->addParent(rule);
							}
						}
					}
				}
			}
		}
	}
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
	ConceptNode *tempN = new ConceptNode(item, m_concepts[item]);
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
	tree = new ConceptNode(CURSOR, m_concepts[CURSOR]);
	tree->addSegmentPath(CURSOR);
	if (hasRecursiveError(*tree))
		throw std::runtime_error("ERREUR !");
}

Map<ConceptList> SemanticDescriptor::getConcepts() const
{
	return m_concepts;
}

ConceptList &SemanticDescriptor::getConcept(const std::string &key)
{
	return m_concepts.get(key);
}

const ConceptList &SemanticDescriptor::getConcept(const std::string &key) const
{
	return m_concepts.get(key);
}


bool SemanticDescriptor::tryGetConcept(const std::string &key, ConceptList &conceptList) const
{
	return m_concepts.try_get(key, conceptList);
}

ConceptNode *SemanticDescriptor::getTree() const
{
	return tree;
}

std::ostream &operator<<(std::ostream &os, const SemanticDescriptor &data)
{
	log(data.getConcepts());
	return (os);
}