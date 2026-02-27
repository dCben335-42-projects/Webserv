#include <sstream>
#include <iostream>
#include "./SemanticDescriptor.hpp"
#include "../utils/utils.hpp"

namespace bnf
{

SemanticDescriptor::SemanticDescriptor() : tree(NULL)
{
}


void SemanticDescriptor::parse(const std::string &fileContent)
{
	try
	{
		std::istringstream iss(fileContent);
		splitBlocks(iss);
		parseBlocks();
	}
	catch (const SemanticException &e)
	{
		std::cout << e;
		delete tree;
		for (Map<Concept *>::iterator it = m_concepts.begin(); it != m_concepts.end(); ++it)
		{
			Vec<ConceptDef *> &defs = (*it).second->getDefinitions();
			for (Vec<ConceptDef *>::iterator conceptIt = defs.begin(); conceptIt != defs.end(); ++conceptIt)
				delete *conceptIt;
			delete (*it).second;
		}
		throw std::runtime_error("semantic descriptor is not valid");
	}
}

SemanticDescriptor::~SemanticDescriptor()
{
	delete tree;
	for (Map<Concept *>::iterator it = m_concepts.begin(); it != m_concepts.end(); ++it)
	{
		Vec<ConceptDef *> &defs = (*it).second->getDefinitions();
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
	Logger::log("Error: Concept definition is missing separator '::=' or '|=' for concept name: " + block.substr(0, pos));
	throw ConceptHasNotDefinitionException();
}

void SemanticDescriptor::parseBlocks()
{
	size_t pos;

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


Vec<std::string> SemanticDescriptor::splitwords(const std::string &rule)
{
	Vec<std::string> result;
	std::string		 current;

	bool in_single_quote = false;
	bool in_double_quote = false;

	for (size_t i = 0; i < rule.size(); ++i)
	{
		char c = rule[i];

		if (c == '\\' && i + 1 < rule.size())
		{
			char next = rule[i + 1];

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
				current += '\\';
				current += next;
			}

			++i;
			continue;
		}

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

bool SemanticDescriptor::parseAndConsumePrefix(std::string &str, int *out)
{
	size_t i = 0;

	if (str.empty())
		return false;

	if (str[0] == '"')
		throw DoubleQuoteStartException();

	if (!std::isdigit(str[0]))
		return false;

	int value = 0;
	while (i < str.size() && std::isdigit(str[i]))
	{
		value = value * 10 + (str[i] - '0');
		++i;
	}

	if (i >= str.size() || str[i] != '>')
	{
		throw BadPrefixException();
	}
	if (i + 1 >= str.size() || str[i + 1] == '"')
		throw DoubleQuoteStartException();

	str.erase(0, i + 1);

	*out = value;
	return true;
}

void SemanticDescriptor::parseRule(const std::string &m_content, size_t cursor)
{
	Concept	   *cpt		   = NULL;
	ConceptDef *conceptDef = new ConceptDef(m_lastKey);
	if (!m_concepts.try_get(m_lastKey, cpt))
	{
		cpt = new Concept(m_lastKey);
		m_concepts.set(m_lastKey, cpt);
	}

	size_t conceptDefIndex = cpt->getDefinitions().size();
	cpt->pushDef(conceptDef);

	const Vec<std::string> words = splitwords(m_content.substr(cursor));

	std::string word;
	ARule	   *arule	   = NULL;
	size_t		aruleIndex = 0;
	int			n		   = -1;


	for (size_t i = 0; i < words.size(); ++i)
	{
		word = words[i];
		parseAndConsumePrefix(word, &n);
		if (ARule::isKeyWord(word, true))
			throw InvalidKeywordException(word);
		BnfMap map = ARule::getMap(word);
		arule	   = map.constructor(m_lastKey, conceptDefIndex, aruleIndex, cpt, conceptDef);

		if (map.hasClosure)
		{
			if (i + 1 == words.size())
			{
				delete arule;
				throw UnexpectedEndRuleException(map.opener);
			}
			i++;
			word = words[i];
		}
		while (word != map.closer)
		{
			parseAndConsumePrefix(word, &n);
			if (ARule::isKeyWord(word, false))
			{
				delete arule;
				throw BadClosingKeywordException(word);
			}
			if (word != DIVIDER)
			{
				if (!validate_string_strict(word))
				{
					Logger::log("Error: Invalid concept name: " + word + " at index " + Logger::get(i) + " in rule: " + m_content.substr(cursor));
					delete arule;
					throw InvalidConceptNameException(word);
				}
				arule->addItem(TerminalSymbol::specify(word));

				if (n >= 0)
				{
					arule->addRequiredInJson(n, word);
					n = -1;
				}

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
		const Vec<ConceptDef *> &defs	   = (*it).second->getDefinitions();
		const size_t			 defsCount = defs.size();

		for (size_t o = 0; o < defsCount; ++o)
		{
			ConceptDef *conceptIt = defs[o];

			const ARulePath &rules		= conceptIt->getValuesConstRef();
			const size_t	 rulesCount = rules.size();

			for (size_t b = 0; b < rulesCount; ++b)
			{
				Concept					  *conceptPtr;
				const ARule				  *rule	 = rules[b];
				const Vec<TerminalSymbol> &items = rule->getItems();

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
				throw ConceptNotFoundException(items[i].getValue());

			conceptPathsCopy = conceptPaths;
			conceptPathsCopy.push_back(items[i].getValue());

			const Vec<ConceptDef *> &defs	   = conceptPtr->getDefinitions();
			const size_t			 defsCount = defs.size();
			for (size_t j = 0; j < defsCount; ++j)
				ndw(defs[j]->getARule(0), conceptPathsCopy);
			continue;
		}
		for (size_t j = 0; j < conceptPaths.size(); ++j)
			m_concepts.get(conceptPaths[j])->addTerminalChar(rule);
	}
}

void SemanticDescriptor::findAllNext()
{
	Vec<std::string> conceptPaths;

	for (Map<Concept *>::iterator it = m_concepts.begin(); it != m_concepts.end(); ++it)
	{
		conceptPaths.push_back((*it).first);
		const Vec<ConceptDef *> &defs	   = (*it).second->getDefinitions();
		const size_t			 defsCount = defs.size();
		for (size_t i = 0; i < defsCount; ++i)
			ndw(defs[i]->getARule(0), conceptPaths);
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
				throw ConceptNotFoundException(item.getValue());

			if (!m_recursiveConceptAuthorize && (isInContainer(node.getPath(), item.getValue())))
				throw RecursiveConceptException(item.getValue());

			hasInfRecursivity += isInContainer(node.getPath(), item.getValue()) || hasRecursiveErrorOnNewNode(item.getValue(), &node);
		}
		if (!hasInfRecursivity)
			node.setHasInfRecursivity(false);
	}
	return node.hasInfRecursivity();
}

void SemanticDescriptor::checkValidity()
{
	try
	{
		tree = new ConceptNode(CURSOR, m_concepts[CURSOR]->getDefinitions());
	}
	catch (std::bad_alloc &)
	{
		throw MemoryAllocationFailedException();
	}
	tree->addSegmentPath(CURSOR);

	m_recursiveConceptAuthorize = false;
	if (m_recursiveConceptAuthorize == false)
	{
		for (Map<Concept *>::iterator it = m_concepts.begin(); it != m_concepts.end(); ++it)
		{
			std::string conceptName = it->first;
			ConceptList definitions = it->second->getDefinitions();

			for (ConceptList::const_iterator conceptIt = definitions.begin(); conceptIt != definitions.end(); ++conceptIt)
			{
				Vec<TerminalSymbol> items = (*conceptIt)->getItemsValueWithOption();
				for (Vec<TerminalSymbol>::iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
				{
					if (itemIt->isTerminalChar())
						continue;
					else if (itemIt->getValue() == conceptName)
					{
						throw RecursiveConceptException(conceptName);
					}
				}
			}
		}
	}

	if (hasRecursiveError(*tree))
		throw RecursiveLoopConceptException();
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

} // namespace bnf
