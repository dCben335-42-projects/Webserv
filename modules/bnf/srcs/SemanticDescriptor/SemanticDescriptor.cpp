#include <sstream>
#include <iostream>
#include "./SemanticDescriptor.hpp"
#include "../utils/utils.hpp"

using namespace bnf;

/**
 * @brief Constructs a SemanticDescriptor object by parsing the given string content.
 * @param m_content The string content representing the semantic descriptor to be parsed.
 * @throws std::runtime_error if the semantic descriptor is not valid.
 */
SemanticDescriptor::SemanticDescriptor(const std::string &m_content) : tree(NULL), m_recursiveConceptAuthorize(false)
{
	try
	{
		std::istringstream iss(m_content);
		splitBlocks(iss);
		parseBlocks();
	}
	catch (const SemanticException &e)
	{
		std::cout << e;
		clear();
		throw std::runtime_error("semantic descriptor is not valid");
	}
}

/**
 * @brief Destructor for the SemanticDescriptor class.
 */
SemanticDescriptor::~SemanticDescriptor()
{
	clear();
}

/**
 * @brief Gets the ARuleMap representing the starting paths of the semantic descriptor.
 * @return The ARuleMap containing the starting paths.
 */
void SemanticDescriptor::clear()
{
	delete tree;
	tree = NULL;
	for (Map<Concept *>::iterator it = m_concepts.begin(); it != m_concepts.end(); ++it)
	{
		const Vec<ConceptDef *> &defs	   = (*it).second->getDefinitions();
		size_t					 defsCount = defs.size();
		for (size_t i = 0; i < defsCount; ++i)
			delete defs[i];
		delete (*it).second;
	}
	m_concepts.clear();
}

/**
 * @brief Checks if the given line represents the start of a rule.
 * @param line The line to be checked.
 * @param startDef A pointer to a size_t variable where the start position of the definition will be stored.
 * @return true if the line represents the start of a rule, false otherwise.
 */
bool SemanticDescriptor::isStartRule(const std::string &line, size_t *startDef)
{
	const size_t size = line.size();
	size_t		 i	  = 0;

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

/**
 * @brief Splits the input stream into blocks based on the start of rules.
 * @param iss The input string stream containing the semantic descriptor.
 */
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


/**
 * @brief Finds the separator in the given block and updates the position accordingly.
 * @param block The block of text to be checked.
 * @param pos A reference to a size_t variable where the position of the separator will be stored.
 * @return true if the separator is '::=', false if it is '|='.
 */
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

/**
 * @brief Parses the concept name from the given string and updates the last key accordingly.
 */
void SemanticDescriptor::parseBlocks()
{
	size_t		 pos;
	const size_t blocksCount = blocks.size();
	for (size_t i = 0; i < blocksCount; ++i)
	{
		if (findSeparator(blocks[i], pos))
		{
			parseConceptName(blocks[i].substr(0, pos));
			pos += EQRULE.length();
		}
		parseRule(blocks[i], pos);
	}
	checkForRecursion();
	assignParentRules();
	assignTerminalChar();
}


/**
 * @brief Splits the given rule string into individual words, considering quotes and escape sequences.
 * @param rule The rule string to be split.
 * @return A vector of strings representing the individual words in the rule.
 */
Vec<std::string> SemanticDescriptor::splitwords(std::string rule)
{
	Vec<std::string> result;
	std::string		 current;

	bool in_single_quote = false;
	bool in_double_quote = false;

	const size_t ruleSize = rule.size();
	for (size_t i = 0; i < ruleSize; ++i)
	{
		char c = rule[i];

		if (c == '\\' && i + 1 < ruleSize)
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

/**
 * @brief Parses and consumes a prefix from the given string.
 * @param str The string to be parsed.
 * @param out A pointer to an integer where the parsed value will be stored.
 * @return true if a valid prefix was found and consumed, false otherwise.
 */
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
		throw BadPrefixException();
	if (i + 1 >= str.size() || str[i + 1] == '"')
		throw DoubleQuoteStartException();

	str.erase(0, i + 1);

	*out = value;
	return true;
}


/**
 * @brief Parses a rule from the given content starting at the specified cursor position.
 * @param m_content The content containing the rule to be parsed.
 * @param cursor The position in the content where the rule starts.
 */
void SemanticDescriptor::parseRule(const std::string &m_content, size_t cursor)
{
	Concept	   *cpt		   = NULL;
	ConceptDef *conceptDef = new ConceptDef();
	if (!m_concepts.try_get(m_lastKey, cpt))
	{
		cpt = new Concept(m_lastKey);
		m_concepts.set(m_lastKey, cpt);
	}

	const size_t conceptDefIndex = cpt->getDefinitions().size();
	cpt->pushDef(conceptDef);

	const Vec<std::string> &words = splitwords(m_content.substr(cursor));
	std::string				word;
	ARule				   *arule	   = NULL;
	size_t					aruleIndex = 0;
	int						n		   = -1;

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
}

/**
 * @brief Adds parent relationships for all concepts in the semantic descriptor.
 */
void SemanticDescriptor::assignParentRules()
{
	for (Map<Concept *>::iterator it = m_concepts.begin(); it != m_concepts.end(); ++it)
	{
		const Vec<ConceptDef *> &defs	   = (*it).second->getDefinitions();
		const size_t			 defsCount = defs.size();

		for (size_t o = 0; o < defsCount; ++o)
		{
			ConceptDef *conceptIt = defs[o];

			const ARulePath &rules		= conceptIt->getRules();
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

/**
 * @brief Recursively processes the given rule and updates the concept paths.
 * @param rule The ARule to be processed.
 * @param conceptPaths A vector of strings representing the current concept paths being explored.
 */
void SemanticDescriptor::findTerminalChar(const ARule *rule, Vec<std::string> &conceptPaths)
{
	Concept					  *conceptPtr;
	const Vec<TerminalSymbol> &items = rule->getItems();
	size_t					   i_max = items.size();

	for (size_t i = 0; i < i_max; ++i)
	{
		const TerminalSymbol &item = items[i];
		if (!item.isTerminalChar())
		{
			const std::string &value = item.getValue();
			if (conceptPaths.contains(value))
				continue;
			if (!m_concepts.try_get(value, conceptPtr))
				throw ConceptNotFoundException(value);
			conceptPaths.push_back(value);
			const Vec<ConceptDef *> &defs	   = conceptPtr->getDefinitions();
			const size_t			 defsCount = defs.size();
			for (size_t j = 0; j < defsCount; ++j)
				findTerminalChar(defs[j]->getARuleUnsafe(0), conceptPaths);
			conceptPaths.pop_back();
			continue;
		}
		size_t j_max = conceptPaths.size();
		for (size_t j = 0; j < j_max; ++j)
			m_concepts.get(conceptPaths[j])->addTerminalChar(rule);
	}
}

/**
 * @brief Finds all terminal characters for each concept in the semantic descriptor.
 */
void SemanticDescriptor::assignTerminalChar()
{
	Vec<std::string> conceptPaths;

	for (Map<Concept *>::iterator it = m_concepts.begin(); it != m_concepts.end(); ++it)
	{
		conceptPaths.push_back((*it).first);
		const Vec<ConceptDef *> &defs	   = (*it).second->getDefinitions();
		const size_t			 defsCount = defs.size();
		for (size_t i = 0; i < defsCount; ++i)
			findTerminalChar(defs[i]->getARuleUnsafe(0), conceptPaths);
		conceptPaths.clear();
	}
}


ARuleMap SemanticDescriptor::getStartPath() const
{
	return m_startPath;
}

/**
 * @brief Parses the concept name from the given string and updates the last key accordingly.
 * @param s The string containing the concept name to be parsed.
 * @throws EmptyConceptNameException if the concept name is empty.
 * @throws ConceptHasNotDefinitionException if the concept does not have a definition.
 * @throws ConceptAlreadyDefinedException if the concept has already been defined.
 */
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

/**
 * @brief Checks for recursive errors when adding a new node to the ConceptNode tree.
 * @param item The name of the concept being added as a new node.
 * @param parentPtr A pointer to the parent ConceptNode to which the new node will be added.
 * @return true if a recursive error is detected, false otherwise.
 */
bool SemanticDescriptor::hasRecursiveErrorOnNewNode(std::string item, ConceptNode *parentPtr)
{
	ConceptNode *tempN = new ConceptNode(item, m_concepts[item]->getDefinitions());

	tempN->setPath(parentPtr->getPath())
		.addSegmentPath(item)
		.setParent(parentPtr);

	return hasRecursiveError(*tempN);
}

/**
 * @brief Checks for recursive errors in the ConceptNode tree by traversing the nodes and their concepts.
 * @param node The ConceptNode to be checked for recursive errors.
 * @return true if a recursive error is detected, false otherwise.
 * @throws ConceptNotFoundException if a concept referenced in the node is not found in the semantic descriptor.
 * @throws RecursiveConceptException if a recursive concept is detected in the node.
 */
bool SemanticDescriptor::hasRecursiveError(ConceptNode &node)
{
	unsigned int			 hasInfRecursivity;
	const Vec<ConceptDef *> &concepts = node.getConcepts();
	const size_t			 i_max	  = concepts.size();
	for (size_t i = 0; i < i_max; ++i)
	{
		const Vec<TerminalSymbol> &items = concepts[i]->getItems();
		hasInfRecursivity				 = 0;
		const size_t j_max				 = items.size();

		for (size_t j = 0; j < j_max; ++j)
		{
			const TerminalSymbol &item = items[j];

			if (item.isTerminalChar())
				continue;
			const std::string &value = item.getValue();

			if (!isInMap(m_concepts, value))
				throw ConceptNotFoundException(value);

			const Vec<std::string> &path = node.getPath();
			if (!m_recursiveConceptAuthorize && (isInContainer(path, value)))
				throw RecursiveConceptException(value);
			hasInfRecursivity += isInContainer(path, value) || hasRecursiveErrorOnNewNode(value, &node);
		}
		if (!hasInfRecursivity)
			node.setHasInfRecursivity(false);
	}
	return node.hasInfRecursivity();
}

/**
 * @brief Checks the validity of the semantic descriptor by constructing a ConceptNode tree and verifying for recursive errors.
 * @throws RecursiveConceptException if a recursive concept is detected in the definitions.
 * @throws RecursiveLoopConceptException if a recursive loop is detected in the definitions.
 */
void SemanticDescriptor::checkForRecursion()
{
	tree = new ConceptNode(CURSOR, m_concepts[CURSOR]->getDefinitions());
	tree->addSegmentPath(CURSOR);

	if (m_recursiveConceptAuthorize == false)
	{
		for (Map<Concept *>::iterator it = m_concepts.begin(); it != m_concepts.end(); ++it)
		{
			const std::string &conceptName = it->first;
			const ConceptList &definitions = it->second->getDefinitions();

			for (ConceptList::const_iterator conceptIt = definitions.begin(); conceptIt != definitions.end(); ++conceptIt)
			{
				const Vec<TerminalSymbol> &items = (*conceptIt)->getItemsValueWithOption();
				for (Vec<TerminalSymbol>::const_iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
				{
					if (itemIt->isTerminalChar())
						continue;
					else if (itemIt->getValue() == conceptName)
						throw RecursiveConceptException(conceptName);
				}
			}
		}
	}

	if (hasRecursiveError(*tree))
		throw RecursiveLoopConceptException();
}


/**
 * @brief Retrieves the map of concepts defined in the semantic descriptor.
 * @return A map where the keys are concept names and the values are pointers to Concept objects.
 */
Map<Concept *> SemanticDescriptor::getConcepts() const
{
	return m_concepts;
}

/**
 * @brief Retrieves a concept by its name from the semantic descriptor.
 * @param key The name of the concept to retrieve.
 * @return A reference to a pointer to the Concept object.
 */
Concept *&SemanticDescriptor::getConcept(const std::string &key)
{
	return m_concepts.get(key);
}

/**
 * @brief Retrieves a concept by its name from the semantic descriptor.
 * @param key The name of the concept to retrieve.
 * @return A pointer to the Concept object.
 */
const Concept *SemanticDescriptor::getConcept(const std::string &key) const
{
	return m_concepts.get(key);
}

/**
 * @brief Retrieves the ConceptNode tree of the semantic descriptor.
 * @return A pointer to the root ConceptNode.
 */
ConceptNode *SemanticDescriptor::getTree() const
{
	return tree;
}

/**
 * @brief Retrieves the definitions of a concept by its name from the semantic descriptor.
 * @param conceptName The name of the concept whose definitions are to be retrieved.
 * @return A reference to a vector of pointers to ConceptDef objects representing the definitions of the specified concept.
 */
const Vec<ConceptDef *> &SemanticDescriptor::getConceptDefs(const std::string &conceptName) const
{
	return m_concepts.get(conceptName)->getDefinitions();
}


/**
 * @brief Overloads the stream insertion operator to output the semantic descriptor.
 * @param os The output stream.
 * @param data The semantic descriptor to output.
 * @return The output stream.
 */
std::ostream &operator<<(std::ostream &os, const SemanticDescriptor &data)
{
	Logger::log(data.getConcepts());
	return (os);
}