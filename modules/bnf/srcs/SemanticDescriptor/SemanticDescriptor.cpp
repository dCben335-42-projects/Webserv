#include <sstream>
#include <iostream>
#include "./SemanticDescriptor.hpp"
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
			delete *conceptIt;
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


// Vec<std::string> SemanticDescriptor::splitwords(std::string rule)
// {
// 	// Split '(', ')', '{', '}', '[', ']' and '|' if they are not inside simple or double quotes
// 	// split words that are not inside simple or double quotes, if they are inside quotes, keep them together, the whitespace make part of the word

// }

#include <string>
#include <vector>
#include <cctype>

Vec<std::string> SemanticDescriptor::splitwords(std::string rule)
{
	Vec<std::string> result;
	std::string		 current;

	bool in_single_quote = false;
	bool in_double_quote = false;

	for (size_t i = 0; i < rule.size(); ++i)
	{
		char c = rule[i];

		/* Gestion du backslash */
		if (c == '\\' && i + 1 < rule.size())
		{
			char next = rule[i + 1];

			/* Séquences échappées */
			if (next == 'n')
				current += '\n';
			else if (next == 't')
				current += '\t';
			else if (next == 'r')
				current += '\r';
			else if (next == ' ')
				current += ' ';
			else if (next == '\\')
				current += '\\';
			else if (next == '"' || next == '\'')
				current += next;
			else
			{
				/* backslash inutile → littéral */
				current += '\\';
				current += next;
			}

			++i; /* on consomme le caractère suivant */
			continue;
		}

		/* Quotes */
		if (c == '\'' && !in_double_quote)
		{
			in_single_quote = !in_single_quote;
			current += c;
			if (!in_single_quote)
			{
				result.push_back(current);
				current.clear();
			}
			continue;
		}
		if (c == '"' && !in_single_quote)
		{
			in_double_quote = !in_double_quote;
			current += c;
			if (!in_double_quote)
			{
				result.push_back(current);
				current.clear();
			}
			continue;
		}

		/* Hors quotes */
		if (!in_single_quote && !in_double_quote)
		{
			if (std::isspace((unsigned char) c))
			{
				if (!current.empty())
				{
					result.push_back(current);
					current.clear();
				}
				continue;
			}

			if (c == '(' || c == ')' ||
				c == '{' || c == '}' ||
				c == '[' || c == ']' ||
				c == '|')
			{
				if (!current.empty())
				{
					result.push_back(current);
					current.clear();
				}
				result.push_back(std::string(1, c));
				continue;
			}
		}

		current += c;
	}

	if (!current.empty())
		result.push_back(current);

	return result;
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

	const size_t conceptDefIndex = concept->getDefinitions().size();
	concept->pushDef(conceptDef);

	const Vec<std::string> words = splitwords(m_content.substr(cursor));
	Logger::log(words);

	std::string word;
	ARule	   *arule	   = NULL;
	size_t		aruleIndex = 0;

	for (size_t i = 0; i < words.size(); ++i)
	{
		word = words[i];
		if (ARule::isKeyWord(word, true))
			throw std::runtime_error("Invalid keyword: " + word);
		BnfMap map = ARule::getMap(word);
		arule	   = map.constructor(m_lastKey, conceptDefIndex, aruleIndex, concept, conceptDef);


		if (map.hasClosure)
		{
			if (i + 1 == words.size())
				throw std::runtime_error("Unexpected end of rule after " + map.opener);
			i++;
			word = words[i];
		}
		while (word != map.closer)
		{
			if (ARule::isKeyWord(word, false))
				throw std::runtime_error("Bad closing keyword: " + word);
			if (word != DIVIDER)
			{
				if (!validate_string_strict(word))
					throw InvalidConceptNameException(word);
				arule->addItem(TerminalSymbol::specify(word));
				if (!map.hasClosure)
					break;
			}
			i++;
			if (i == words.size())
				break;
			else
				word = words[i];
		}
		conceptDef->setRule(arule);
		aruleIndex++;
	}


	conceptDef->finish();
	conceptDef->computeOptionalIndex();
}

void SemanticDescriptor::addParent()
{
	for (Map<Concept *>::iterator it = m_concepts.begin(); it != m_concepts.end(); ++it)
	{
		V_FOREACH(ConceptDef *, conceptIt, (*it).second->getDefinitions())
		{
			V_FOREACH(const ARule *, ruleIt, (*conceptIt)->getValues())
			{
				Concept			   *conceptPtr;
				const ARule		   *rule  = *ruleIt;
				Vec<TerminalSymbol> items = rule->getItems();

				for (size_t i = 0; i < items.size(); ++i)
				{
					if (m_concepts.try_get(items[i].getValue(), conceptPtr))
						conceptPtr->addParent(rule);
				}
			}
		}
	}
}

void SemanticDescriptor::ndw(const ARule *rule, Vec<std::string> &conceptPaths)
{
	Concept			   *conceptPtr;
	Vec<std::string>	conceptPathsCopy;
	Vec<TerminalSymbol> items = rule->getItems();
	for (size_t i = 0; i < items.size(); ++i)
	{
		if (!items[i].isTerminalChar() && conceptPaths.contains(items[i].getValue()))
			continue;
		if (!items[i].isTerminalChar())
		{
			if (!m_concepts.try_get(items[i].getValue(), conceptPtr))
				throw std::runtime_error(ERR_CONCEPT_NOT_FOUND + items[i].getValue());
			conceptPathsCopy = conceptPaths;
			conceptPathsCopy.push_back(items[i].getValue());
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
}

ARuleMap SemanticDescriptor::getStartPath() const
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
	Vec<TerminalSymbol> items;
	unsigned int		hasInfRecursivity;

	for (ConceptList::const_iterator it = node.getConcepts().begin(); it != node.getConcepts().end(); ++it)
	{
		items			  = (*it)->getItems();
		hasInfRecursivity = 0;

		for (Vec<TerminalSymbol>::iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
		{
			const TerminalSymbol &item = *itemIt;

			if (item.isTerminalChar())
				continue;

			if (!isInMap(m_concepts, item.getValue()))
				throw std::runtime_error(ERR_CONCEPT_NOT_FOUND + item.getValue());

			hasInfRecursivity += isInContainer(node.getPath(), item.getValue()) || hasRecursiveErrorOnNewNode(item.getValue(), &node);
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

ConceptNode *SemanticDescriptor::getTree() const
{
	return tree;
}

const Vec<ConceptDef *> &SemanticDescriptor::getConceptDefs(const std::string &conceptName) const
{
	return m_concepts.get(conceptName)->getDefinitions();
}

std::ostream &operator<<(std::ostream &os, const SemanticDescriptor &data)
{
	Logger::log(data.getConcepts());
	return (os);
}