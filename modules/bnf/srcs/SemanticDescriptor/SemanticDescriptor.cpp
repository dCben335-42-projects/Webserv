#include <sstream>
#include <iostream>
#include "./SemanticDescriptor.hpp"
#include "Vector.hpp"
#include "Logger.hpp"
#include "../utils/utils.hpp"

using namespace bnf;

SemanticDescriptor::SemanticDescriptor(): tree(NULL) {}

SemanticDescriptor::SemanticDescriptor(const std::string& m_content)
{
    std::istringstream iss(m_content);
    splitBlocks(iss);
    parseBlocks();
}

SemanticDescriptor::~SemanticDescriptor() {
    delete tree;

    for (Map<ConceptList>::iterator it = m_concepts.begin(); it != m_concepts.end(); ++it) {
        for (Vec<Concept*>::iterator conceptIt = it->second.begin(); conceptIt != it->second.end(); ++conceptIt) {
            delete *conceptIt;
        }
    }
}

bool SemanticDescriptor::isStartRule(const std::string &line, size_t *startDef)
{
    size_t size = line.size();
    size_t i = 0;
    
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
    size_t startDef = 0, prevStartDef = 0;

    while (std::getline(iss, line))
    {
        if (isStartRule(line, &startDef) && !cblock.empty()) {
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
    if ((pos = block.find(EQRULE)) != std::string::npos) {
        return true;
    }

    if ((pos = block.find(ORRULE)) != std::string::npos) {
        pos += ORRULE.length();
        return false;
    }

    throw ConceptHasNotDefinitionException();
}

void SemanticDescriptor::parseBlocks() {
    size_t pos;
    
    try {
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
    } catch (const SemanticException &e) { std::cout << e; }
}


void SemanticDescriptor::parseRule(const std::string &m_content, size_t cursor)
{
    Concept *concept = new Concept();
    m_concepts[m_lastKey].push_back(concept);
    std::istringstream iss(m_content.substr(cursor));
    std::string word;
    ARule *arule = NULL;

    while (iss >> word)
    {
        if (ARule::isKeyWord(word, true))
            throw std::runtime_error("Invalid keyword: " + word);
        BnfMap map = ARule::getMap(word);
        arule = map.constructor();
        if (map.hasClosure && !(iss >> word))
            throw std::runtime_error("Unexpected end of rule after " + map.opener);
        while (word != map.closer)
        {
            if (ARule::isKeyWord(word, false))
                throw std::runtime_error("Bad closing keyword: " + word);
            if (word != DIVIDER)
            {
                if(!validate_string_strict(word))
                    throw InvalidConceptNameException(word); 
                arule->addItem(word);
                if (!map.hasClosure)
                    break;
            }
            if (!(iss >> word))
                break;
        }
        concept->setRule(arule);
    }
}

void SemanticDescriptor::parseConceptName(std::string s)
{
    std::istringstream iss(erasePaternWrapper(s, WS));
    std::string w;
    if (!(iss >> w))
        throw EmptyConceptNameException();
    if (iss >> w)
        throw ConceptHasNotDefinitionException();
    if (isInMap(m_concepts, s))
        throw ConceptAlreadyDefinedException(s);
    m_lastKey = s;
}

bool SemanticDescriptor::hasRecursiveErrorOnNewNode(std::string item, TreeNode *parentPtr)
{
        TreeNodeData data(item, m_concepts[item]);
        TreeNode* tempNode = new TreeNode(findOrInsert(m_data, item, data)->second);
        tempNode->setPath(parentPtr->path).addSegmentPath(item).setParent(parentPtr);
        return hasRecursiveError(tempNode);
}

bool SemanticDescriptor::hasRecursiveError(TreeNode *node)
{
    for (ConceptList::iterator it = node->content.concepts.begin(); it != node->content.concepts.end(); ++it)
    {
        Vec<std::string>::type items = (*it)->getItems();
        unsigned int hasInfRecursivity = 0;

        for (Vec<std::string>::iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
        {
            const std::string &item = *itemIt;

            if (startsWith(item, DQSQ))
                continue;

            if (!isInMap(m_concepts, item))
                throw std::runtime_error(ERR_CONCEPT_NOT_FOUND + item);

            hasInfRecursivity += isInContainer(node->path, item) || hasRecursiveErrorOnNewNode(item, node);
        }
        if (!hasInfRecursivity)
            node->content.isInfinite = false;
    }
    return node->content.isInfinite;
}

void SemanticDescriptor::checkValidity()
{
    Vec<std::string>::type path;
    TreeNodeData data(CURSOR, m_concepts[CURSOR]);
    tree = new TreeNode(data);
    tree->addSegmentPath(CURSOR);
    if (hasRecursiveError(tree))
        throw  std::runtime_error("ERREUR !");
}

Map<ConceptList>::type SemanticDescriptor::getConcepts() const
{
    return m_concepts;
}

TreeNode *SemanticDescriptor::getTree() const
{
    return tree;
}

std::ostream& operator<<(std::ostream& os, const SemanticDescriptor& data)
{
    log(data.getConcepts());
    return (os);
}